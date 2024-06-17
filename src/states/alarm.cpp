#include <pitches.h>

#include "lcd_wrapper.h"
#include "rtc_wrapper.h"
#include "config.h"
#include "states.h"

enum state state_alarm() {
    lcd_clear();

    lcd_print_at(1, 4, "Wake Up!");

    char buffer_clock[12];

    while(1) {
        sprintf(buffer_clock, "%02u:%02u:%02u", get_hours(), get_minutes(), get_seconds());
        lcd_print_at(0, 4, buffer_clock);

        if(state_update())
            break;

        tone(BUZZER_PIN, NOTE_E6, 500);
        delay(500);
        noTone(BUZZER_PIN);
        delay(500);
    }

    return CLOCK;
}