#include "LiquidCrystal.h"
#include "ESP8266WiFi.h"
#include "WiFiUdp.h"
#define SerialMon Serial
#include "AppleMIDI.h"
#include "ESP8266mDNS.h"

#define PLAYLIST_MAX 60
#define PLAYLIST_STRING_LENGTH 17

extern LiquidCrystal lcd;
extern char playlist[PLAYLIST_MAX][PLAYLIST_STRING_LENGTH];
extern u_int16_t song_id[PLAYLIST_MAX];

void playlist_goto_id(u_int16_t id);
void playlist_goto_index(u_int16_t index);
void display_connect_info();

unsigned long t0 = millis();
int8_t isConnected = 0;

APPLEMIDI_CREATE_INSTANCE(WiFiUDP, MIDI, "MidiTrigger", DEFAULT_CONTROL_PORT);


void setup_rtp()
{
    // Set up mDNS responder:
    if (!MDNS.begin(AppleMIDI.getName()))
        AM_DBG(F("Error setting up MDNS responder!"), 0x0032);
    AM_DBG(F("mDNS responder started ("), AppleMIDI.getName(), ".local)");
    MDNS.addService("apple-midi", "udp", AppleMIDI.getPort());

    AM_DBG(F("OK, now make sure you an rtpMIDI session that is Enabled"));
    AM_DBG(F("Add device named Arduino with Host"), WiFi.localIP(), "Port", AppleMIDI.getPort(), "(Name", AppleMIDI.getName(), ")");
    AM_DBG(F("Select and then press the Connect button"));
    AM_DBG(F("Then open a MIDI listener and monitor incoming notes"));
    AM_DBG(F("Listen to incoming MIDI commands"));

    MIDI.begin();

    // Connection handlers
    AppleMIDI.setHandleConnected([](const APPLEMIDI_NAMESPACE::ssrc_t & ssrc, const char* name) {
        isConnected++;
        AM_DBG(F("Connected to session"), ssrc, name);
        playlist_goto_index(0);
    });
    AppleMIDI.setHandleDisconnected([](const APPLEMIDI_NAMESPACE::ssrc_t & ssrc) {
        isConnected--;
        AM_DBG(F("Disconnected"), ssrc);
        if (isConnected == 0) {
            display_connect_info();
        }
    });
    
    // Note handlers
    MIDI.setHandleControlChange([](byte channel, byte number, byte value) {
        AM_DBG(F("CC"), value);
        if (channel == 1) {
            playlist_goto_id((u_int16_t)number*100 + (u_int16_t)value);
        }
    });
    MIDI.setHandleNoteOn([](byte channel, byte note, byte velocity) {
        AM_DBG(F("NoteOn"), note);
    });
    MIDI.setHandleNoteOff([](byte channel, byte note, byte velocity) {
        AM_DBG(F("NoteOff"), note);
    });

    display_connect_info();
}

void display_connect_info()
{
    lcd.setCursor(0, 0);
    lcd.print("Rtp? Port:      ");
    lcd.setCursor(11, 0);
    lcd.print(AppleMIDI.getPort());

    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    lcd.print("    ");
}

void loop_rtp()
{
    // Listen to incoming notes
    MIDI.read();
    // Update mDNS to promote to rtpMidi directories
    MDNS.update();
}

void rtp_send_play_stop()
{
    byte note = 0;
    byte velocity = 127;
    byte channel = 2;

    MIDI.sendNoteOn(note, velocity, channel);
    MIDI.sendNoteOff(note, velocity, channel);
}

void rtp_send_next()
{
    byte note = 1;
    byte velocity = 127;
    byte channel = 2;

    MIDI.sendNoteOn(note, velocity, channel);
    MIDI.sendNoteOff(note, velocity, channel);
}

void rtp_send_prev()
{
    byte note = 2;
    byte velocity = 127;
    byte channel = 2;

    MIDI.sendNoteOn(note, velocity, channel);
    MIDI.sendNoteOff(note, velocity, channel);
}
