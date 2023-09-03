#include "LiquidCrystal.h"
#include "LittleFS.h"
#include "ESP8266HTTPClient.h"
#include "WiFiClientSecure.h"
#include "paths.hpp"

extern LiquidCrystal lcd;

#define PLAYLIST_MAX 60
#define PLAYLIST_STRING_LENGTH 17
char playlist[PLAYLIST_MAX][PLAYLIST_STRING_LENGTH];
u_int16_t song_id[PLAYLIST_MAX];

//const char *host = "https://192.168.178.1:15201/nas/filelink.lua?id=8be6da70eefd36df";
//const uint8_t fingerprint[] = {0x28, 0x31, 0x19, 0x9B, 0xD9, 0x77, 0xD1, 0x1D, 0x92, 0xF9, 0x11, 0xB7, 0x84, 0xA1, 0xD9, 0xFB, 0xC3, 0x6D, 0x38, 0xB1};

void download_playlist(char* host, u_int8_t* fingerprint)
{
    // Get file from server
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);

    if (fingerprint[0] == 0 && fingerprint[1] == 0) { // If the first two values are empty
        Serial.println("[HTTPS] Insecure request...");
        client->setInsecure();
    } else {
        Serial.printf("[HTTPS] Using fingerprint %02x:%02x:%02x:%02x:...\n", fingerprint[0], fingerprint[1], fingerprint[2], fingerprint[3]);
        client->setFingerprint(fingerprint);
    }

    HTTPClient https;

    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, host))
    { // HTTPS

        Serial.print("[HTTPS] GET...\n");
        // start connection and send HTTP header
        int httpCode = https.GET();

        // httpCode will be negative on error
        if (httpCode > 0)
        {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

            // file found at server
            if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
            {
                String payload = https.getString();
                File tmp = LittleFS.open(PLAYLIST_PATH, "r"); // open for reading just for file size
                u_int16_t old_size = tmp.size();
                tmp.close();

                if (payload.length() != old_size)
                {
                    Serial.printf("[HTTPS] Different size to backup (old: %d, new: %d): save to backup file...\n", old_size, payload.length());
                    File write = LittleFS.open(PLAYLIST_PATH, "w"); // now open for writing
                    write.print(payload);
                    delay(1);
                    write.close();
                } else {
                    Serial.println("[HTTPS] Nothing changed; keeping old backup file.");
                }
            }
        }
        else
        {
            Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
            lcd.setCursor(0, 0);
            lcd.print("HTTP err: ");
            lcd.print(httpCode);
            lcd.print("     ");
            delay(3000); // 3 seonds
        }

        https.end();
    }
    else
    {
        Serial.println("[HTTPS] Unable to connect");
        lcd.setCursor(0, 0);
        lcd.print("HTTP connect err");
        delay(3000); // 3 seonds
    }
}

void setup_playlist()
{
    lcd.setCursor(0, 0);
    lcd.print("Wifi! Data?     ");

    // Read backup file
    LittleFS.begin();

    // Try to download the playlist (the method will update the playlist file)
    File file = LittleFS.open(PLAYLIST_URL_PATH, "r");
    char host[301] = "";
    file.readBytes(host, 300);
    file.close();

    file = LittleFS.open(PLAYLIST_URL_FINGERPRINT_PATH, "r");
    u_int8_t fingerprint[20] = {};
    for (u_int8_t i = 0; file.available() && i < 20;)
    {
        char tmp[5] = "0x";
        file.readBytesUntil(':', tmp+2, 2);
        if (tmp[2] != 0) {
            fingerprint[i++] = strtol(tmp, nullptr, 16);
        }
    }
    file.close();

    if (strlen(host) > 0) {
        Serial.printf("[Playlist] Download from host %s\n", host);
        download_playlist(host, (u_int8_t*)fingerprint);
    }

    // Now read playlist file
    Serial.println("[Playlist] Read backup file");
    // Read playlist into array
    file = LittleFS.open(PLAYLIST_PATH, "r");
    for (int i = 0; file.available() && i < PLAYLIST_MAX; i++)
    {
        // Read the id
        char id_buffer[7] = "";
        file.readBytesUntil(',', id_buffer, 6);
        song_id[i] = (u_int16_t)atoi(id_buffer);

        // read the song name
        char name_buffer[41] = "";
        file.readBytesUntil('\n', name_buffer, 40);

        // Copy to playlist array and end the string with null terminator
        strncpy(playlist[i], name_buffer, PLAYLIST_STRING_LENGTH - 1);
        playlist[i][PLAYLIST_STRING_LENGTH - 1] = '\0';

        Serial.printf("[Playlist] Got %d, id %d, %s\n", i, song_id[i], playlist[i]);
    }
    file.close();

    lcd.setCursor(0, 0);
    lcd.print("Data! Rtp?      ");
}

void playlist_display_next_of(u_int16_t index)
{
    index++;

    lcd.setCursor(0, 1);
    lcd.print("                ");

    if (index >= PLAYLIST_MAX) {
        Serial.printf("[Playlist] Warning: Secondary ignoring index %d\n", index);
    }

    lcd.setCursor(2, 1);
    lcd.print(playlist[index]);
}

void playlist_goto_index(u_int16_t index)
{
    Serial.printf("[Playlist] Go to index %d\n", index);

    lcd.setCursor(0, 0);
    lcd.print("                ");

    if (index >= PLAYLIST_MAX) {
        Serial.printf("[Playlist] Warning: Primary ignoring index %d\n", index);
        return;
    }

    lcd.setCursor(0, 0);
    lcd.print(playlist[index]);

    playlist_display_next_of(index);
}

void playlist_goto_id(u_int16_t id)
{
    Serial.printf("[Playlist] Go to id %d\n", id);

    // Search song by ID
    for (u_int16_t i = 0; i < PLAYLIST_MAX; i++) {
        if (song_id[i] == id) {
            playlist_goto_index(i);
            return;
        }
    }

}
