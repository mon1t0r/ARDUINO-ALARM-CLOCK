#include "states.h"
#include "lcd_wrapper.h"
#include "rtc_wrapper.h"
#include "settings_storage.h"
#include "config.h"

#define UPDATE_DURATION 100
#define BLINK_INTERVAL 700

#define BLINK_INTERVAL_COUNT (BLINK_INTERVAL / UPDATE_DURATION)

volatile struct settings_value {
    struct dt dt;
    struct settings settings;
    bool is_dirty;
} settings_value;

volatile uint16_t selected_index;

uint16_t settings_counter;
enum state settings_state;

void change_dt_value(bool increment);
int change_value_in_range(bool increment, int value, int range_start, int range_end);

void get_settings();
void set_settings();

void state_settings_init(enum state state) {
    lcd_clear();

    struct dt now_dt = now();

    settings_value.dt.day = now_dt.day;
    settings_value.dt.month = now_dt.month;
    settings_value.dt.year = now_dt.year;
    settings_value.dt.hours = now_dt.hours;
    settings_value.dt.minutes = now_dt.minutes;
    settings_value.dt.seconds = now_dt.seconds;

    if(state == SETTINGS_DATETIME || state == SETTINGS_ALARM || state == SETTINGS_OTHER) {
        get_settings();
    }

    settings_value.is_dirty = 1;
    
    settings_state = state;
    selected_index = 0;
    settings_counter = 0;
}

void state_settings_handle_button_press(byte btn_state) {
    switch (btn_state) {
        case BTN1_PRESSED:
            if(settings_state == SETTINGS) {
                selected_index = change_value_in_range(0, selected_index, 0, 3);
            } else if(settings_state == SETTINGS_OTHER) {
                selected_index = change_value_in_range(0, selected_index, 0, 2);
            } else {
                change_dt_value(0);
            }
            break;
        
        case BTN2_PRESSED:
            if(settings_state == SETTINGS) {
                selected_index = change_value_in_range(1, selected_index, 0, 3);
            } else if(settings_state == SETTINGS_OTHER) {
                selected_index = change_value_in_range(1, selected_index, 0, 2);
            } else {
                change_dt_value(1);
            }
            break;
        
        case BTN3_PRESSED:
            if(settings_state == SETTINGS) {
                if(selected_index != 3) {
                    request_state_finish((enum state)(SETTINGS_DATETIME + selected_index));
                } else {
                    request_state_finish(NONE);
                }
            } else if(settings_state == SETTINGS_OTHER) {
                switch (selected_index) {
                    case 0:
                        settings_value.settings.alarm_enabled = !settings_value.settings.alarm_enabled;
                        break;

                    case 1:
                        settings_value.settings.ldr_enabled = !settings_value.settings.ldr_enabled;
                        break;

                    case 2:
                        request_state_finish(SETTINGS_NFC);
                        break;
                    
                    default:
                        break;
                }
                settings_value.is_dirty = 1;
            } else {
                selected_index = change_value_in_range(1, selected_index, 0,
                                settings_state == SETTINGS_ALARM ? 1 : 5);
            }
            break;

        case BTN4_PRESSED:
            if(settings_state != SETTINGS) {
                request_state_finish(NONE);
            }
            break;
    }
}

enum state state_settings() {
    state_settings_init(SETTINGS);

    lcd_print_at(0, 1, "Time");
    lcd_print_at(0, 11, "Alarm");
    lcd_print_at(1, 1, "Other");
    lcd_print_at(1, 11, "Back");

    while(1) {
        if(settings_counter % BLINK_INTERVAL_COUNT == 0) {
            for(size_t i = 0; i < 4; ++i) {
                lcd_print_at((i / 2), (i % 2) * 10, " ");
            }
        }

        if((settings_counter + BLINK_INTERVAL_COUNT / 2) % BLINK_INTERVAL_COUNT == 0) {
            lcd_print_at((selected_index / 2), (selected_index % 2) * 10, ">");
        }

        if(state_update())
            break;

        if(++settings_counter >= BLINK_INTERVAL_COUNT) {
            settings_counter = 0;
        }
            
        delay(UPDATE_DURATION);
    }

    return CLOCK;
}

enum state state_settings_datetime() {
    state_settings_init(SETTINGS_DATETIME);

    lcd_print_at(0, 0, "Time:");
    lcd_print_at(1, 0, "Date:");

    char buffer_clock[12];

    while(1) {
        if(settings_value.is_dirty || settings_counter % BLINK_INTERVAL_COUNT == 0) {
            sprintf(buffer_clock, "%02u:%02u:%02u", settings_value.dt.hours, settings_value.dt.minutes, settings_value.dt.seconds);
            lcd_print_at(0, 6, buffer_clock);
            
            sprintf(buffer_clock, "%02u.%02u.%d", settings_value.dt.day, settings_value.dt.month, settings_value.dt.year);
            lcd_print_at(1, 6, buffer_clock);

            settings_value.is_dirty = 0;
        }

        if((settings_counter + BLINK_INTERVAL_COUNT / 2) % BLINK_INTERVAL_COUNT == 0) {
            lcd_print_at(
                (selected_index / 3),
                6 + (selected_index % 3) * 3,
                selected_index % 3 == 2 ? "    " : "  "
            );
        }

        if(state_update())
            break;

        if(++settings_counter >= BLINK_INTERVAL_COUNT) {
            settings_counter = 0;
        }
            
        delay(UPDATE_DURATION);
    }

    set_datetime(settings_value.dt.day, settings_value.dt.month, settings_value.dt.year,
        settings_value.dt.hours, settings_value.dt.minutes, settings_value.dt.seconds);

    return CLOCK;
}

enum state state_settings_alarm() {
    state_settings_init(SETTINGS_ALARM);

    settings_value.dt.hours = settings_value.settings.alarm_hours;
    settings_value.dt.minutes = settings_value.settings.alarm_minutes;

    lcd_print_at(0, 0, "Alarm:");
    
    char buffer_clock[12];

    while(1) {
        if(settings_value.is_dirty || settings_counter % BLINK_INTERVAL_COUNT == 0) {
            sprintf(buffer_clock, "%02u:%02u", settings_value.dt.hours, settings_value.dt.minutes);
            lcd_print_at(0, 7, buffer_clock);

            settings_value.is_dirty = 0;
        }

        if((settings_counter + BLINK_INTERVAL_COUNT / 2) % BLINK_INTERVAL_COUNT == 0) {
            lcd_print_at(0, 7 + (selected_index % 3) * 3, "  ");
        }

        if(state_update())
            break;

        if(++settings_counter >= BLINK_INTERVAL_COUNT) {
            settings_counter = 0;
        }
            
        delay(UPDATE_DURATION);
    }

    settings_value.settings.alarm_hours = settings_value.dt.hours;
    settings_value.settings.alarm_minutes = settings_value.dt.minutes;

    set_settings();
    settings_save();
    
    return CLOCK;
}

enum state state_settings_other() {
    state_settings_init(SETTINGS_OTHER);

    lcd_print_at(0, 0, "Alr:");
    lcd_print_at(0, 9, "LDR:");
    lcd_print_at(1, 3, "NFC-Setup");

    while(1) {
        if(settings_value.is_dirty || settings_counter % BLINK_INTERVAL_COUNT == 0) {
            for(size_t i = 0; i < 2; ++i) {
                bool value;
                switch (i) {
                    case 0:
                        value = settings_value.settings.alarm_enabled;
                        break;

                    case 1:
                        value = settings_value.settings.ldr_enabled;
                        break;
                    
                    default:
                        break;
                }

                lcd_print_at(i / 2, 4 + (i % 2) * 9, value ? "ON" : "OFF");
            }

            lcd_print_at(1, 2, selected_index == 2 ? ">" : " ");
            
            settings_value.is_dirty = 0;
        }

        if((settings_counter + BLINK_INTERVAL_COUNT / 2) % BLINK_INTERVAL_COUNT == 0) {
            if(selected_index != 2) {
                lcd_print_at(selected_index / 2, 4 + (selected_index % 2) * 9, "   ");
            } else {
                lcd_print_at(1, 2, " ");
            }
        }

        if(state_update())
            break;

        if(++settings_counter >= BLINK_INTERVAL_COUNT) {
            settings_counter = 0;
        }
            
        delay(UPDATE_DURATION);
    }

    set_settings();
    settings_save();
    
    return CLOCK;
}

void change_dt_value(bool increment) {
    switch (selected_index) {
        case 0:
            settings_value.dt.hours = change_value_in_range(increment, settings_value.dt.hours, 0, 23);
            break;

        case 1:
            settings_value.dt.minutes = change_value_in_range(increment, settings_value.dt.minutes, 0, 59);
            break;

        case 2:
            settings_value.dt.seconds = change_value_in_range(increment, settings_value.dt.seconds, 0, 59);
            break;

        case 3:
            settings_value.dt.day = change_value_in_range(increment, settings_value.dt.day, 1, 31);
            break;

        case 4:
            settings_value.dt.month = change_value_in_range(increment, settings_value.dt.month, 1, 12);
            break;

        case 5:
            settings_value.dt.year = change_value_in_range(increment, settings_value.dt.year, 2000, 2150);
            break;
    }
    settings_value.is_dirty = 1;
}

int change_value_in_range(bool increment, int value, int range_start, int range_end) {
    if(increment && ++value > range_end) {
        value = range_start;
    } else if(!increment && --value < range_start) {
        value = range_end;
    }
    return value;
}

void get_settings() {
    struct settings *settings = settings_get();

    settings_value.settings.alarm_hours =
        settings->alarm_hours < 24 ? settings->alarm_hours : 0;

    settings_value.settings.alarm_minutes =
        settings->alarm_minutes < 60 ? settings->alarm_minutes : 0;

    settings_value.settings.alarm_enabled = settings->alarm_enabled;
    settings_value.settings.ldr_enabled = settings->ldr_enabled;
}

void set_settings() {
    struct settings *settings = settings_get();

    settings->alarm_hours = settings_value.settings.alarm_hours;
    settings->alarm_minutes = settings_value.settings.alarm_minutes;

    settings->alarm_enabled = settings_value.settings.alarm_enabled;
    settings->ldr_enabled = settings_value.settings.ldr_enabled;
}
