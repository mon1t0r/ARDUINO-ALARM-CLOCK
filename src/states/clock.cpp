#include "states.h"
#include "lcd_wrapper.h"
#include "rtc_wrapper.h"

enum state state_clock() {
    lcd_clear();

    char buffer_clock[12];

    for(size_t i = 0; i < 10; ++i) {
        sprintf(buffer_clock, "%02u:%02u:%02u", get_hours(), get_minutes(), get_seconds());
        lcd_print_at(0, 4, buffer_clock);
        
        sprintf(buffer_clock, "%02u.%02u.%d", get_day(), get_month(), get_year());
        lcd_print_at(1, 3, buffer_clock);

        if(state_update())
            break;

        delay(1000);
    }

    return ENV;
}