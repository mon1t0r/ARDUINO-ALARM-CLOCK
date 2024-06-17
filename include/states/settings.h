#include "states.h"

void state_settings_handle_button_press(byte btn_state);

enum state state_settings();
enum state state_settings_datetime();
enum state state_settings_alarm();
enum state state_settings_other();