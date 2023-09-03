#include "Wire.h"
#include "LiquidCrystal.h"

extern LiquidCrystal lcd;

void setup_display();
void setup_buttons();
void setup_wifi();
void setup_playlist();
void setup_rtp();
void setup_web();
void setup_ota();
void loop_display();
void loop_buttons();
void loop_wifi();
void loop_rtp();
void loop_web();
void loop_ota();

void setup()
{
  Serial.begin(115200);
  Serial.println("Hallo!");

  setup_display();
  setup_buttons();
  setup_wifi();
  setup_playlist();
  setup_rtp();
  setup_web();
  setup_ota();
}

void loop()
{
  loop_display();
  loop_buttons();
  loop_wifi();
  loop_rtp();
  loop_web();
  loop_ota();
}
