#include "LiquidCrystal.h"
#include "ESP8266WiFi.h"
#include "WiFiUdp.h"
#define SerialMon Serial
#include "AppleMIDI.h"

#define PLAYLIST_MAX 60
#define PLAYLIST_STRING_LENGTH 17

extern LiquidCrystal lcd;
extern char playlist[PLAYLIST_MAX][PLAYLIST_STRING_LENGTH];
extern u_int16_t song_id[PLAYLIST_MAX];

void playlist_goto_id(u_int16_t id);

unsigned long t0 = millis();
int8_t isConnected = 0;

APPLEMIDI_CREATE_INSTANCE(WiFiUDP, MIDI, "MidiTrigger", DEFAULT_CONTROL_PORT);


void setup_rtp()
{
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
        playlist_goto_id(1);
    });
    AppleMIDI.setHandleDisconnected([](const APPLEMIDI_NAMESPACE::ssrc_t & ssrc) {
        isConnected--;
        AM_DBG(F("Disconnected"), ssrc);
        lcd.setCursor(0, 1);
        lcd.print("LOST RTP! Waiting");
    });
    
    // Note handlers
    MIDI.setHandleNoteOn([](byte channel, byte note, byte velocity) {
        AM_DBG(F("NoteOn"), note);
        playlist_goto_id((u_int16_t)note*100 + (u_int16_t)velocity);
    });
    MIDI.setHandleNoteOff([](byte channel, byte note, byte velocity) {
        AM_DBG(F("NoteOff"), note);
    });
}

void loop_rtp()
{
    // Listen to incoming notes
    MIDI.read();
}

void rtp_send_play_stop()
{
    byte note = 0;
    byte velocity = 127;
    byte channel = 1;

    MIDI.sendNoteOn(note, velocity, channel);
    MIDI.sendNoteOff(note, velocity, channel);
}

void rtp_send_next()
{
    byte note = 1;
    byte velocity = 127;
    byte channel = 1;

    MIDI.sendNoteOn(note, velocity, channel);
    MIDI.sendNoteOff(note, velocity, channel);
}

void rtp_send_prev()
{
    byte note = 2;
    byte velocity = 127;
    byte channel = 1;

    MIDI.sendNoteOn(note, velocity, channel);
    MIDI.sendNoteOff(note, velocity, channel);
}
