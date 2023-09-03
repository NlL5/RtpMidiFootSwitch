#include "LiquidCrystal.h"
#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>

extern LiquidCrystal lcd;

void setup_ota()
{
    ArduinoOTA.onStart([]() {
        String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
        Serial.println("Start updating " + type);
        lcd.setCursor(0, 0);
        lcd.print("Up:             ");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        lcd.setCursor(4, 0);
        for (u_int8_t i = 0; i < (progress / total) * 13; i++) {
            lcd.print("#");
        }
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");

        lcd.setCursor(0, 0);
        lcd.print("OTA update err:");
        lcd.print(error);
    });

    ArduinoOTA.setPassword("93485oisufdg");
    ArduinoOTA.begin();
}

void loop_ota()
{
    // Check for over the air update request and (if present) flash it
    ArduinoOTA.handle();
}
