#include "debug.h"
#include "scanner.h"
#include "sensitive.h"
#include <Wire.h>
#include <WiFi.h>

//#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define PIN_OUT_SCAN_STATUS_LED BUILTIN_LED
#define PIN_BUT 14
#define PIN_LED2 21

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
    pinMode(PIN_OUT_SCAN_STATUS_LED, OUTPUT);
    pinMode(PIN_BUT, INPUT_PULLUP);
    pinMode(PIN_LED2, OUTPUT);
    debug_setup();
    scanner_setup();
    wifi_connect_builtin();
}

void blink_led(int pin) {
    digitalWrite(pin, HIGH);
    delay(50); // TODO ugh
    digitalWrite(pin, LOW);
}

int button_prev = LOW;
int led_state = LOW;
void toggle_led_if_state_change(int pin1, int pin2) {
    int button_next = digitalRead(pin1);
    if (button_prev == HIGH && button_next == LOW) {
        if (led_state == LOW) {
            log("but", "going high");
            digitalWrite(pin2, HIGH);
            led_state = HIGH;
        } else if (led_state == HIGH) {
            log("but", "going low");
            digitalWrite(pin2, LOW);
            led_state = LOW;
        }
    }
    button_prev = button_next;
}

void loop(void) {
    log("run");
    blink_led(PIN_OUT_SCAN_STATUS_LED);
    //scanner_scan();
    toggle_led_if_state_change(PIN_BUT, PIN_LED2);
    delay(2000);
}
