// ============================================================
//  Pulse Oximeter → ThingSpeak IoT Monitor
//  Hardware : ESP32 + MAX30100
//  I2C Pins : SDA = GPIO4 (D4) | SCL = GPIO5 (D5)
// ============================================================

#include <WiFi.h>
#include <Wire.h>
#include "ThingSpeak.h"
#include "MAX30100_PulseOximeter.h"

// ----- Pin Definitions ----------------------------------------
#define I2C_SDA 4
#define I2C_SCL 5

// ----- Network Credentials ------------------------------------
//  ⚠️  Move these to a separate "secrets.h" file before sharing!
const char* WIFI_SSID     = "YOUR_SSID";
const char* WIFI_PASSWORD = "YOUR_PASSWORD";

// ----- ThingSpeak Credentials ---------------------------------
//  ⚠️  Rotate your API key if this code is ever made public.
const unsigned long TS_CHANNEL  = 3272996;
const char*         TS_API_KEY  = "YOUR_WRITE_API_KEY";

// ----- Timing Constants (ms) ----------------------------------
static const uint32_t SERIAL_INTERVAL   =  1000;   // 1 s  – serial print
static const uint32_t THINGSPEAK_INTERVAL = 16000; // 16 s – ThingSpeak minimum
static const uint32_t WIFI_TIMEOUT      = 20000;   // 20 s – WiFi connect timeout

// ----- Sanity-check thresholds --------------------------------
static const float MIN_VALID_BPM  = 40.0f;
static const float MAX_VALID_BPM  = 220.0f;
static const float MIN_VALID_SPO2 = 70.0f;

// ----- Globals ------------------------------------------------
WiFiClient  wifiClient;
PulseOximeter pox;

uint32_t lastSerialMs     = 0;
uint32_t lastThingSpeakMs = 0;

// ==============================================================
//  Helpers
// ==============================================================

bool connectWiFi() {
    Serial.print("Connecting to WiFi");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    uint32_t startMs = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - startMs > WIFI_TIMEOUT) {
            Serial.println("\n[ERROR] WiFi connection timed out.");
            return false;
        }
        delay(500);
        Serial.print(".");
    }

    Serial.printf("\n[OK] WiFi connected – IP: %s\n",
                  WiFi.localIP().toString().c_str());
    return true;
}

bool isReadingValid(float bpm, float spo2) {
    return (bpm  >= MIN_VALID_BPM  && bpm  <= MAX_VALID_BPM) &&
           (spo2 >= MIN_VALID_SPO2 && spo2 <= 100.0f);
}

// ==============================================================
//  setup()
// ==============================================================

void setup() {
    Serial.begin(115200);
    delay(200);
    Serial.println("\n=== Pulse Oximeter Monitor – Starting ===");

    // I2C
    Wire.begin(I2C_SDA, I2C_SCL, 100000);

    // WiFi
    if (!connectWiFi()) {
        Serial.println("[WARN] Continuing without WiFi – cloud upload disabled.");
    } else {
        ThingSpeak.begin(wifiClient);
    }

    // MAX30100 sensor
    Serial.println("Initialising MAX30100...");
    if (!pox.begin()) {
        Serial.println("[FATAL] MAX30100 not found – check wiring on D4/D5.");
        while (true) { delay(1000); } // halt
    }

    pox.setIRLedCurrent(MAX30100_LED_CURR_14_2MA);
    Serial.println("[OK] MAX30100 ready.\n");
}

// ==============================================================
//  loop()
// ==============================================================

void loop() {
    pox.update(); // must be called as fast as possible

    uint32_t now = millis();

    // ------ Serial output every SERIAL_INTERVAL ---------------
    if (now - lastSerialMs >= SERIAL_INTERVAL) {
        lastSerialMs = now;

        float bpm  = pox.getHeartRate();
        float spo2 = pox.getSpO2();

        Serial.printf("BPM: %5.1f  |  SpO2: %4.1f%%  |  Valid: %s\n",
                      bpm, spo2,
                      isReadingValid(bpm, spo2) ? "YES" : "NO (finger on sensor?)");
    }

    // ------ ThingSpeak upload every THINGSPEAK_INTERVAL -------
    if (now - lastThingSpeakMs >= THINGSPEAK_INTERVAL) {
        lastThingSpeakMs = now;

        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("[WARN] WiFi lost – attempting reconnect...");
            connectWiFi();
            return;
        }

        float bpm  = pox.getHeartRate();
        float spo2 = pox.getSpO2();

        if (!isReadingValid(bpm, spo2)) {
            Serial.println("[SKIP] Invalid reading – ThingSpeak upload skipped.");
            return;
        }

        ThingSpeak.setField(1, bpm);
        ThingSpeak.setField(2, spo2);

        int httpCode = ThingSpeak.writeFields(TS_CHANNEL, TS_API_KEY);
        if (httpCode == 200) {
            Serial.printf("[CLOUD] Uploaded → BPM: %.1f  SpO2: %.1f%%\n", bpm, spo2);
        } else {
            Serial.printf("[CLOUD] Upload failed – HTTP %d\n", httpCode);
        }
    }
}
