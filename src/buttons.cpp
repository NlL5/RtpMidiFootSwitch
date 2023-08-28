#include "Wire.h"
#include "FastIO.h"
#include "LiquidCrystal.h"

extern LiquidCrystal lcd;

void rtp_send_play_stop();
void rtp_send_next();
void rtp_send_prev();
void press_ctrl();
void reset_ctrl();

#define BUTTON_0 D5
#define BUTTON_1 D0
u_int8_t button_state = 0;
unsigned long ctrl_pressed = 0;
unsigned long left_pressed = 0;
unsigned long right_pressed = 0;

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

    if (changed)
    {
        Serial.printf("[Buttons] Old: %x, new: %x\n", button_state, button_state_new);
    }

    button_state = button_state_new;

    // handle buttons:
    if (changed & 0b1)
    {
        // left button
        left_pressed = millis();
    }
    else if (changed & 0b10)
    {
        // right button
        right_pressed = millis();
    }

    // entprellen 100ms
    if (left_pressed && millis() > left_pressed + 100)
    {
        left_pressed = 0;

        if (!ctrl_pressed)
        {
            press_ctrl();
        }
        else
        {
            reset_ctrl();
            rtp_send_prev();
        }
    }
    if (right_pressed && millis() > right_pressed + 100)
    {
        right_pressed = 0;

        if (!ctrl_pressed)
        {
            rtp_send_play_stop();
        }
        else
        {
            reset_ctrl();
            rtp_send_next();
        }
    }

    // Reset ctrl after 5 seconds
    if (ctrl_pressed && millis() > ctrl_pressed + 5000)
    {
        reset_ctrl();
    }
}

void reset_ctrl()
{
    ctrl_pressed = 0;
    lcd.setCursor(0, 1);
    lcd.print(" ");
}

void press_ctrl()
{
    lcd.setCursor(0, 1);
    lcd.print("? ");
    ctrl_pressed = millis();
}
