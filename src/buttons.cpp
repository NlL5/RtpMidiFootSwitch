#include "Wire.h"
#include "FastIO.h"
#include "LiquidCrystal.h"

extern LiquidCrystal lcd;

#define BUTTON_0 D5
#define BUTTON_1 D0
u_int8_t button_state = 0;

void setup_buttons()
{
    pinMode(BUTTON_0, INPUT_PULLUP);
    pinMode(BUTTON_1, INPUT_PULLUP);

    button_state = digitalRead(BUTTON_0);
    button_state |= digitalRead(BUTTON_1) << 1;
}

void loop_buttons()
{
    u_int8_t button_state_new = 0;
    button_state_new = digitalRead(BUTTON_0);
    button_state_new |= digitalRead(BUTTON_1) << 1;
    u_int8_t changed = button_state_new ^ button_state;

    lcd.setCursor(0, 1);
    if (changed & 0b1) {
        lcd.print("L");
    } else if (changed & 0b10) {
        lcd.print("R");
    }

    button_state = button_state_new;
}
