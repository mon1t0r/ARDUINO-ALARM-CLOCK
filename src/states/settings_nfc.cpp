#include <pitches.h>

#include "lcd_wrapper.h"
#include "rtc_wrapper.h"
#include "nfc_wrapper.h"
#include "settings_storage.h"
#include "states.h"
#include "config.h"

#define START_BYTE 0xF1

struct settings_data {
    byte day;
    byte month;
    int year;
    byte hours;
    byte minutes;
    byte seconds;

    byte alarm_hours;
    byte alarm_minutes;
    bool alarm_enabled;
    bool ldr_enabled;
}  __attribute__((__packed__));

uint16_t settings_nfc_counter;

void update_header();

enum state state_settings_nfc() {
    lcd_clear();

    settings_nfc_counter = 0;

    while(1) {
        update_header();

        if(nfc_in_list_passive_target()) {
            settings_nfc_counter = 0;
            update_header();

            lcd_print_at(1, 0, "RECEIVING DATA..");

            uint8_t selectApdu[] = { 
                0x00, /* CLA */
                0xA4, /* INS */
                0x04, /* P1  */
                0x00, /* P2  */
                0x07, /* Length of AID  */
                0xE8, 0xAF, 0x77, 0x81, 0xA1, 0xCE, 0xCC, /* AID */
                0x00  /* Le  */
            };

            uint8_t data_bytes[64];
            uint8_t data_lenght = sizeof(data_bytes) / sizeof(uint8_t);

            bool success = nfc_data_exchange(selectApdu, sizeof(selectApdu), data_bytes, &data_lenght);

            tone(BUZZER_PIN, NOTE_E6, 1000);

            // Serial.print("Success: ");
            // Serial.println(success);
            // Serial.print("Data lenght: ");
            // Serial.println(data_lenght);

            // Serial.print("Bytes: ");
            // for(size_t i = 0; i < data_lenght; ++i) {
            //     Serial.print((int) data_bytes[i]);
            //     Serial.print(' ');
            // }
            // Serial.println();
            // Serial.println();

            if(success && (data_lenght != sizeof(struct settings_data) + 1 || data_bytes[0] != START_BYTE)) {
                success = false;
            }

            if(success) {
                struct settings_data *data = (struct settings_data *) (data_bytes + 1); // + START_BYTE

                set_datetime(data->day, data->month, data->year, data->hours, data->minutes, data->seconds);

                struct settings *settings = settings_get();
                settings->alarm_hours = data->alarm_hours;
                settings->alarm_minutes = data->alarm_minutes;
                settings->alarm_enabled = data->alarm_enabled;
                settings->ldr_enabled = data->ldr_enabled;

                settings_save();

                delay(1000);
            } else {
                noTone(BUZZER_PIN);

                for(size_t i = 0; i < 3; ++i) {
                    delay(100);
                    tone(BUZZER_PIN, NOTE_E6, 200);
                    delay(100);
                    noTone(BUZZER_PIN);
                }
            }

            noTone(BUZZER_PIN);

            if(success) {
                lcd_print_at(1, 0, " DATA RECEIVED! ");
            } else {
                lcd_print_at(1, 0, "RECEIVE FAILED");
            }

            delay(1000);

            char command_clear[17];
            for(size_t i = 0; i < sizeof(command_clear) - 1; ++i) {
                command_clear[i] = ' ';
            }
            command_clear[16] = 0;
            lcd_print_at(1, 0, command_clear);
        }
        
        if(state_update())
            break;

        delay(500);
    }

    return CLOCK;
}

void update_header() {
    char text[10];
    memcpy(text + 3, "NFC", 3);
    text[0] = CHAR_NFC_LEFT_BIG;
    text[1] = CHAR_NFC_LEFT_MEDIUM;
    text[2] = CHAR_NFC_LEFT_SMALL;

    text[6] = CHAR_NFC_RIGHT_SMALL;
    text[7] = CHAR_NFC_RIGHT_MEDIUM;
    text[8] = CHAR_NFC_RIGHT_BIG;

    text[9] = 0;

    lcd_print_at(0, 3, text);

    uint16_t index = (3 + settings_nfc_counter) % 4;
    lcd_print_at(0, 5 - index, " ");
    lcd_print_at(0, 9 + index, " ");

    settings_nfc_counter++;
    if(settings_nfc_counter >= 4) {
        settings_nfc_counter = 0;
    }
}