#ifndef _STATES_H
#define _STATES_H

#include <Arduino.h>

/**
 * List of available states.
 */
enum state {
    NONE,
    CLOCK,
    ENV,
    ALARM,
    FACTORY_RESET,
    SETTINGS,
    SETTINGS_DATETIME,
    SETTINGS_ALARM,
    SETTINGS_OTHER,
    SETTINGS_NFC
};

void states_init();
bool state_update();
void states_update();

void request_state_finish(enum state _requested_state);

void states_handle_button_press(byte btn_state);

#endif
