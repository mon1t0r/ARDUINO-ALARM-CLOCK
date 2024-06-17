#ifndef _NFC_H
#define _NFC_H

#include <Arduino.h>

void nfc_init();

bool nfc_in_list_passive_target();

bool nfc_data_exchange(uint8_t *adpu, uint8_t adpu_size, uint8_t *response, uint8_t *response_lenght);

#endif
