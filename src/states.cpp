#include <time.h>

#include "config.h"
#include "states.h"

#include "lcd_wrapper.h"
#include "rtc_wrapper.h"
#include "sensors.h"
#include "settings_storage.h"

#include "states/clock.h"
#include "states/env.h"
#include "states/alarm.h"
#include "states/factory_reset.h"
#include "states/settings.h"
#include "states/states_nfc.h"

volatile enum state current_state;
volatile enum state requested_state;

volatile bool is_finish_requested;
bool alarm_flag;

void states_init() {
    current_state = CLOCK;
    requested_state = NONE;
    is_finish_requested = 0;
    alarm_flag = 0;
}

void request_state_finish(enum state _requested_state) {
    is_finish_requested = 1;
    requested_state = _requested_state;
}

void reset_state_finish() {
    is_finish_requested = 0;
}

bool state_update() {
    struct settings *settings = settings_get();
    switch(current_state) {
        // Alarm only in CLOCK or ENV state
        case CLOCK:
        case ENV:
            if(settings->alarm_enabled &&
                    get_hours() == settings->alarm_hours &&
                    get_minutes() == settings->alarm_minutes) {
                if(!alarm_flag) {
                    request_state_finish(ALARM);
                    alarm_flag = 1;
                }
            } else if(alarm_flag) {
                alarm_flag = 0;
            }
            break;

        default:
            break;
    }

    return is_finish_requested;
}

void states_update() {
    lcd_backlight(!settings_get()->ldr_enabled || !is_light());

    switch(current_state) {
        case CLOCK:
            current_state = state_clock();
            break;

        case ENV:
            current_state = state_env();
            break;

        case ALARM:
            current_state = state_alarm();
            break;

        case FACTORY_RESET:
            current_state = state_factory_reset();
            break;

        case SETTINGS:
            current_state = state_settings();
            break;

        case SETTINGS_DATETIME:
            current_state = state_settings_datetime();
            break;
        
        case SETTINGS_ALARM:
            current_state = state_settings_alarm();
            break;

        case SETTINGS_OTHER:
            current_state = state_settings_other();
            break;

        case SETTINGS_NFC:
            current_state = state_settings_nfc();
            break;

        default:
            delay(100);
            break;
    }

    if(requested_state != NONE) {
        current_state = requested_state;
        requested_state = NONE;
    }

    reset_state_finish();
}

void states_handle_button_press(byte btn_state) {
    switch(current_state) {
        case CLOCK:
        case ENV:
            if(btn_state != BTN4_PRESSED) {
                request_state_finish(current_state == CLOCK ? ENV : CLOCK);
                return;
            } else {
                request_state_finish(SETTINGS);
                return;
            }
            break;

        case ALARM:
        case SETTINGS_NFC:
            request_state_finish(NONE);
            break;

        case SETTINGS:
        case SETTINGS_DATETIME:
        case SETTINGS_ALARM:
        case SETTINGS_OTHER:
            state_settings_handle_button_press(btn_state);
            break;

        default:
            break;
    }
}
