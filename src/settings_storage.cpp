#include <EEPROM.h>

#include "settings_storage.h"
#include "config.h"

struct settings settings;

void settings_save() {
    EEPROM.put(SETTINGS_EEPROM_ADDRESS, settings);
}

void settings_load() {
    EEPROM.get(SETTINGS_EEPROM_ADDRESS, settings);
}

struct settings* settings_get() {
    return &settings;
}
