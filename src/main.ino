#include "debug.h"
#include "scanner.h"
#include <Wire.h>
#include <WiFi.h>

//#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define PIN_LED BUILTIN_LED

// Replace with your network credentials
//char* ssid     = "***REMOVED***";
//const char* password = "***REMOVED***";
char* ssid     = "TODO";
const char* password = "TODO";

void wifi_connect_builtin(void) {
    log_begin("net/wifi", "connecting via builtin credentials");
    log("net/wifi", ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        log_indicate_wait();
    }
    log_end();
    log("net/wifi", "connected");
    const char* ip = WiFi.localIP().toString().c_str();
    log("net/wifi", ip);
}

void setup(void) {
    pinMode(PIN_LED, OUTPUT);
    debug_setup();
    scanner_setup();
    //wifi_connect_builtin();
}

void loop(void) {
    digitalWrite(PIN_LED, HIGH);
    log("ping");
    delay(50);
    digitalWrite(PIN_LED, LOW);
    delay(2000);
    scanner_scan();
}
