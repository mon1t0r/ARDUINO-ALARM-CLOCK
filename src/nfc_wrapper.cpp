#include <Arduino.h>
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>

PN532_I2C pn532_i2c(Wire);
PN532 nfc(pn532_i2c);

void nfc_init() {
    nfc.begin();
    nfc.setPassiveActivationRetries(0x0);
    nfc.SAMConfig();
}

bool nfc_in_list_passive_target() {
    return nfc.inListPassiveTarget();
}

bool nfc_data_exchange(uint8_t *adpu, uint8_t adpu_size, uint8_t *response, uint8_t *response_lenght) {
    return nfc.inDataExchange(adpu, adpu_size, response, response_lenght);
}