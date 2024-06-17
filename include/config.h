#define INTERRUPT_PIN 2

#define DHT_PIN 8

#define LDR_PIN A0

#define RTC_CLK_PIN 5
#define RTC_DAT_PIN 4
#define RTC_RST_PIN 9

#define BUZZER_PIN 6

#define LCD_SDA_PIN A4
#define LCD_SCL_PIN A5

#define BTN1_PIN 7
#define BTN2_PIN 10
#define BTN3_PIN 11
#define BTN4_PIN 12

#define BTN1_PRESSED 0b00000001
#define BTN2_PRESSED 0b00000010
#define BTN3_PRESSED 0b00000100
#define BTN4_PRESSED 0b00001000

#define SETTINGS_EEPROM_ADDRESS 0x0

#define LDR_THRESHOLD 700

// duration for entering the factory reset state
#define FACTORY_RESET_INTERVAL 3 * 1000

#define BUTTON_PRESS_REGISTER_INTERVAL 200

// for testing purposes
// #define __DATE__ "1.1.2000"
// #define __TIME__ "00:00:00"
