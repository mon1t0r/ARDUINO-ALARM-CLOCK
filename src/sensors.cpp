#include <DHT.h>

#include "config.h"

DHT dht(DHT_PIN, DHT11);

void sensors_init() {
    dht.begin();
    pinMode(LDR_PIN, INPUT);
}

float get_temperature() {
    return dht.readTemperature();
}

float get_humidity() {
    return dht.readHumidity();
}

bool is_light() {
    return analogRead(LDR_PIN) > LDR_THRESHOLD;
}