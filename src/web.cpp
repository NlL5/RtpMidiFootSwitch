
#include "LiquidCrystal.h"
#include "ESP8266WebServer.h"
#include "LittleFS.h"
#include "paths.hpp"

extern LiquidCrystal lcd;

ESP8266WebServer server(80);

void setup_playlist();
void handleGet();
void handlePost();
void handleNotFound();

void setup_web()
{
    LittleFS.begin(); // make sure, FS is available

    // Callback handlers
    server.on("/", HTTP_GET, handleGet);
    server.on("/save", HTTP_POST, handlePost);
    server.onNotFound(handleNotFound);

    // Actually start the server
    server.begin();

    Serial.println("[HTTP server] Started!");
}

void loop_web()
{
    server.handleClient(); // Listen for HTTP requests from clients
}

void handleGet()
{
    Serial.println("[HTTP server] GET");

    File file = LittleFS.open(PLAYLIST_URL_PATH, "r");
    char host[301] = "";
    file.readBytes(host, 300);
    file.close();

    file = LittleFS.open(PLAYLIST_URL_FINGERPRINT_PATH, "r");
    char fingerprint[61] = "";
    file.readBytes(fingerprint, 60);
    file.close();

    char buffer[1000];
    sprintf(buffer, "<form action=\"/save\" method=\"POST\">"
                "<label>Playlist URL: <input name=\"url\" type=\"text\" value=\"%s\"></label><br>"
                "<label>URL SHA-1 Fingerprint (empty for insecure): <input name=\"fingerprint\" type=\"text\" value=\"%s\"></label><br>"
                "<input type=\"submit\" value=\"Save\">"
                "</form>", host, fingerprint);

    server.send(200, "text/html", buffer);
}

void handlePost()
{
    Serial.println("[HTTP server] POST");

    // Todo: compare with old
    File file = LittleFS.open(PLAYLIST_URL_PATH, "r");
    char old_host[301] = "";
    file.readBytes(old_host, 300);
    file.close();

    file = LittleFS.open(PLAYLIST_URL_FINGERPRINT_PATH, "r");
    char old_fingerprint[61] = "";
    file.readBytes(old_fingerprint, 60);
    file.close();

    File write;
    String new_host = server.arg("url");
    if (strcmp(old_host, new_host.c_str()) != 0) {
        write = LittleFS.open(PLAYLIST_URL_PATH, "w");
        write.print(new_host);
        delay(1);
        write.close();
        Serial.println("[HTTP server] Saved new playlist URL.");
    }

    String new_fingerprint = server.arg("fingerprint");
    if (strcmp(old_fingerprint, new_fingerprint.c_str()) != 0) {
        write = LittleFS.open(PLAYLIST_URL_FINGERPRINT_PATH, "w");
        write.print(new_fingerprint);
        delay(1);
        write.close();
        Serial.println("[HTTP server] Saved new fingerprint.");
    }

    server.sendHeader("Location", "/");
    server.send(303); // Redirect to start page

    // Re-read playlist
    setup_playlist();
}

void handleNotFound()
{
    Serial.println("[HTTP server] Not found");

    server.send(404, "text/plain", "404: Not found");
}
