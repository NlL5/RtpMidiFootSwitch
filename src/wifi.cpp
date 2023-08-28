#include "Wire.h"
#include "LiquidCrystal.h"
#include "WiFiManager.h"

#define STANDALONE_SSID "MidiTrigger"
#define STANDALONE_PASSWORD "93485oisufdg"

extern LiquidCrystal lcd;

WiFiManager wifiManager;

void setup_wifi()
{
    lcd.clear();
    lcd.print("Wifi?");

    WiFi.hostname(STANDALONE_SSID); // For DHCP

    wifiManager.setConfigPortalTimeout(120); // Retry to connect every 2 minutes
    wifiManager.setConfigPortalTimeoutCallback([](void) {
        lcd.print("No Wifi. Retry..");
        Serial.println("Not connected after 120 seconds, restarting ...");
        ESP.restart();
    });

    wifiManager.setAPCallback([](WiFiManager *myWiFiManager) {
        lcd.setCursor(0, 0);
        lcd.print("Wifi? AP mode...");
        lcd.setCursor(0, 1);
        lcd.print(WiFi.softAPIP());
    });

    // Do auto connect
    wifiManager.autoConnect(STANDALONE_SSID, STANDALONE_PASSWORD);

    lcd.clear();
}

void loop_wifi()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        lcd.setCursor(0, 0);
        lcd.print("NO WIFI!! Reboot");
    }
}
