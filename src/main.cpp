#include <Arduino.h>

#include "rtc_wrapper.h"
#include "lcd_wrapper.h"
#include "nfc_wrapper.h"
#include "sensors.h"
#include "states.h"
#include "settings_storage.h"
#include "config.h"

volatile unsigned long last_btn_press;

/**
 * ISR for button press.
 *
 * This function is executed, when button is pressed.
 */
void on_button_press() {
    noInterrupts();

    if(millis() - last_btn_press > BUTTON_PRESS_REGISTER_INTERVAL) {
        byte btn_state = 0;

        if(digitalRead(BTN1_PIN) == HIGH) {
            btn_state |= BTN1_PRESSED;
        }

        if(digitalRead(BTN2_PIN) == HIGH) {
            btn_state |= BTN2_PRESSED;
        }

        if(digitalRead(BTN3_PIN) == HIGH) {
            btn_state |= BTN3_PRESSED;
        }

        if(digitalRead(BTN4_PIN) == HIGH) {
            btn_state |= BTN4_PRESSED;
        }

        states_handle_button_press(btn_state);

        last_btn_press = millis();
    }

    interrupts();
}


void setup() {
    // needed from original main() function
    init();

    last_btn_press = 0;

    Serial.begin(9600);
    // while(!Serial.available()){
    //     delay(100);
    // }

    pinMode(INTERRUPT_PIN, INPUT);
    pinMode(BTN1_PIN, INPUT);
    pinMode(BTN2_PIN, INPUT);
    pinMode(BTN3_PIN, INPUT);
    pinMode(BTN4_PIN, INPUT);

    pinMode(BUZZER_PIN, OUTPUT);

    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), on_button_press, RISING);

    settings_load();

    // init alarm clock modules
    lcd_init();
    clock_init();
    nfc_init();
    sensors_init();
    states_init();
}


int main() {
    setup();

    // main loop
    while(1) {
        // in loop()
        states_update();
    }
}
