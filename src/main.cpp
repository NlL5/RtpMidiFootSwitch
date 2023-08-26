#include "Wire.h"
#include "LiquidCrystal.h"

void setup_display();
void setup_buttons();
void loop_display();
void loop_buttons();

void setup()
{
  setup_display();
  setup_buttons();

  Serial.begin(115200);
  Serial.println("Hallo!");
}

void loop()
{
  loop_display();
  loop_buttons();
}
