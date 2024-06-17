#include "states.h"
#include "lcd_wrapper.h"
#include "sensors.h"

enum state state_env() {
    lcd_clear();

    lcd_print_at(0, 0, "Temp.");
    lcd_print_at(0, 15, "C");

    lcd_print_at(1, 0, "Hum.");
    lcd_print_at(1, 15, "%");

    char buffer_env[5];

    for(size_t i = 0; i < 6; ++i) {
        if(i % 2 == 0) {
            sprintf(buffer_env, "%.1f", (double) get_temperature());
            lcd_print_at(0, 8, buffer_env);

            sprintf(buffer_env, "%.1f", (double) get_humidity());
            lcd_print_at(1, 8, buffer_env);
        }

        if(state_update())
            break;

        delay(1000);
    }

    return CLOCK;
}