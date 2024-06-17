#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "lcd_wrapper.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

byte char_nfc_left_small[] = {
  B00000,
  B00000,
  B00100,
  B01000,
  B01000,
  B00100,
  B00000,
  B00000
};

byte char_nfc_left_medium[] = {
  B00000,
  B00100,
  B01000,
  B01000,
  B01000,
  B01000,
  B00100,
  B00000
};

byte char_nfc_left_big[] = {
  B00100,
  B01000,
  B01000,
  B01000,
  B01000,
  B01000,
  B01000,
  B00100
};

byte char_nfc_right_small[] = {
  B00000,
  B00000,
  B00100,
  B00010,
  B00010,
  B00100,
  B00000,
  B00000
};

byte char_nfc_right_medium[] = {
  B00000,
  B00100,
  B00010,
  B00010,
  B00010,
  B00010,
  B00100,
  B00000
};

byte char_nfc_right_big[] = {
  B00100,
  B00010,
  B00010,
  B00010,
  B00010,
  B00010,
  B00010,
  B00100
};

void lcd_init() {
    lcd.init();

    lcd.createChar(CHAR_NFC_LEFT_SMALL, char_nfc_left_small);
    lcd.createChar(CHAR_NFC_LEFT_MEDIUM, char_nfc_left_medium);
    lcd.createChar(CHAR_NFC_LEFT_BIG, char_nfc_left_big);
    lcd.createChar(CHAR_NFC_RIGHT_SMALL, char_nfc_right_small);
    lcd.createChar(CHAR_NFC_RIGHT_MEDIUM, char_nfc_right_medium);
    lcd.createChar(CHAR_NFC_RIGHT_BIG, char_nfc_right_big);

    lcd.backlight();
}

void lcd_clear() {
    lcd.clear();
}


void lcd_set_cursor(int y, int x) {
    lcd.setCursor(x, y);
}

void lcd_print(const char* text) {
    lcd.print(text);
}

void lcd_print_at(int y, int x, const char* text) {
    lcd_set_cursor(y, x);
    lcd_print(text);
}

void lcd_backlight(bool state) {
    if(state)
        lcd.backlight();
    else
        lcd.noBacklight();
}