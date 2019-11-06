#include "debug.h"
#include "scanner.h"
#include "config.h"
#include "sensitive.h"

#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "esp_system.h"

#define TIMER_COUNT_UP true
#define TIMER_EDGE_TRIGGER true
#define TIMER_AUTORELOAD true
#define TIMER_INC_US 80
#define US_TO_MS 1000

#define TIMER_BLINKER_NUM 0
#define TIMER_BLINKER_TIMEOUT_MS 1000

int total_interrupts;

volatile boolean i_t_blinker;
portMUX_TYPE i_t_blinker_mutex = portMUX_INITIALIZER_UNLOCKED;
volatile boolean i_g_button;
portMUX_TYPE i_g_button_mutex = portMUX_INITIALIZER_UNLOCKED;

hw_timer_t *timer_blinker = NULL;

void IRAM_ATTR i_t_blinker_cb() {
    portENTER_CRITICAL_ISR(&i_t_blinker_mutex);
    i_t_blinker = true;
    portEXIT_CRITICAL_ISR(&i_t_blinker_mutex);
}

void IRAM_ATTR i_g_button_cb() {
    portENTER_CRITICAL_ISR(&i_g_button_mutex);
    i_g_button = true;
    portEXIT_CRITICAL_ISR(&i_g_button_mutex);
}

void interrupts_setup() {
    timer_blinker = timerBegin(TIMER_BLINKER_NUM, TIMER_INC_US, TIMER_COUNT_UP);
    timerAttachInterrupt(timer_blinker, &i_t_blinker_cb, TIMER_EDGE_TRIGGER);
    timerAlarmWrite(timer_blinker, TIMER_BLINKER_TIMEOUT_MS * US_TO_MS, TIMER_AUTORELOAD);
    timerAlarmEnable(timer_blinker);

    pinMode(PIN_BUT, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_BUT), i_g_button_cb, FALLING);
}

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
    log("net/wifi", WiFi.localIP().toString().c_str());
}

void wifi_off(void) {
    WiFi.mode(WIFI_OFF);
}

void wifi_disconnect(void) {
    WiFi.disconnect();
}

void radio_setup(void) {
    btStop();
    wifi_off();
}

void setup(void) {
    pinMode(PIN_OUT_SCAN_STATUS_LED, OUTPUT);
    pinMode(PIN_LED2, OUTPUT);
    debug_setup();

    radio_setup();

    interrupts_setup();
    scanner_setup();
}

const char* mqtt_server = "mqtt.eclipse.org";
int mqtt_port = 1883;

WiFiClient mqtt_wifi_client;
PubSubClient mqtt_client(mqtt_wifi_client);

void mqtt_setup() {
    mqtt_client.setServer(mqtt_server, mqtt_port);
}

boolean mqtt_try_connect() {
    log("mqtt", "connecting...");
    if (mqtt_client.connect("chippy")) {
        log("mqtt", "connected");
        mqtt_send_test_msg();
        //mqtt_client.subscribe("raehik");
    } else {
        log("mqtt", "failed to connect");
        Serial.print("failed, rc=");
        Serial.println(mqtt_client.state());
    }
    return mqtt_client.connected();
}

void mqtt_send_test_msg(void) {
    mqtt_client.publish("raehik", "chippy calling in");
}

void mqtt_disconnect() {
    mqtt_client.disconnect();
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
    if (i_t_blinker) {
        portENTER_CRITICAL(&i_t_blinker_mutex);
        i_t_blinker = false;
        portEXIT_CRITICAL(&i_t_blinker_mutex);

        total_interrupts++;
        blink_led(PIN_OUT_SCAN_STATUS_LED);
    } else if (i_g_button) {
        portENTER_CRITICAL(&i_g_button_mutex);
        i_g_button = false;
        portEXIT_CRITICAL(&i_g_button_mutex);

        //blink_led(PIN_LED2);
        wifi_connect_builtin();
        mqtt_try_connect();
        while (!mqtt_client.connected()) {
            log("mqtt", "retrying connection after delay");
            delay(2000);
            mqtt_try_connect();
        }
        mqtt_send_test_msg();
        mqtt_disconnect();
        wifi_disconnect();
        wifi_off();
    }
}
