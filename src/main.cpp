#include "Wire.h"
#include "LiquidCrystal.h"

extern LiquidCrystal lcd;

void setup_display();
void setup_buttons();
void setup_wifi();
void download_playlist();
void loop_display();
void loop_buttons();

void setup()
{
  Serial.begin(115200);
  Serial.println("Hallo!");

  setup_display();
  setup_buttons();
  setup_wifi();
  download_playlist();
}

void loop()
{
  loop_display();
  loop_buttons();
}
