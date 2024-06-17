#ifndef _SENSORS_H
#define _SENSORS_H

/**
 * Sensors initialization.
 */
void sensors_init();


/**
 * Get the temperature
 *
 * @return The temperature in °C.
 */
float get_temperature();


/**
 * Get the humidity
 *
 * @return Value of humidity in the range from 0 (dry) to 100 (wet).
 */
float get_humidity();

bool is_light();

#endif
