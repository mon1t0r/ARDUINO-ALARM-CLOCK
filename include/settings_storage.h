#ifndef _SETTINGS_H
#define _SETTINGS_H

#include <Arduino.h>

struct settings {
    byte alarm_hours;
    byte alarm_minutes;
    bool alarm_enabled;
    bool ldr_enabled;
};


void settings_save();

void settings_load();

struct settings* settings_get();


#endif
