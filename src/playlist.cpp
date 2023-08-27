#include "LiquidCrystal.h"
#include "FTPClient.h"
#include "LittleFS.h"
#define SPIFFS LittleFS
#define PLAYLIST_PATH "playlist.csv"

extern LiquidCrystal lcd;

void download_playlist()
{
    lcd.setCursor(0, 0);
    lcd.print("Wifi! Data?     ");

    // Create file
    File tmp = LittleFS.open(PLAYLIST_PATH, "a");
    tmp.close();

    // Get file from server
    FTPClient ftpClient(LittleFS);
    FTPClient::ServerInfo ftpServerInfo("MidiTrigger", "VADGV$wm%9K\%X8AK4o^7o", "192.168.178.1", 21);
    ftpClient.begin(ftpServerInfo);
    ftpClient.transfer(PLAYLIST_PATH, PLAYLIST_PATH, FTPClient::FTP_GET); // get a file blocking
    ftpClient.stop();

    // Read playlist file
    lcd.setCursor(0, 0);
    File file = LittleFS.open(PLAYLIST_PATH, "r");
    while(file.available()) {
        u_int8_t current = file.read();
        lcd.write(current);
    }
    file.close();

    lcd.setCursor(0, 0);
    //lcd.print("Data! Base?     ");
}