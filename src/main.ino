#include "states.h"
#include "config.h"

void setup(void) {
    randomSeed(micros());
    hardware_setup();
    debug_setup();
}

void hardware_setup() {
    btStop();
    pinMode(PIN_OUT_TAG_READ_STATUS_LED, OUTPUT);
    pinMode(PIN_LED2, OUTPUT);
}

void loop(void) {
    switch(state) {
        case STATE_POWERON:
            state_poweron();
            break;
        case STATE_READY:
            state_ready();
            break;
        case STATE_READ:
            state_read();
            break;
        case STATE_IDLE:
            state_idle();
            break;
        case STATE_SLEEP:
            state_sleep();
            break;
        case STATE_TAGCHECK:
            state_tagcheck();
            break;
    }
}






#include "nfc.h"
#include "sensitive.h"

#include <NfcTag.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "esp_system.h"

#define TIMER_COUNT_UP true
#define TIMER_EDGE_TRIGGER true
#define TIMER_AUTORELOAD true
#define TIMER_INC_US 80
#define US_TO_MS 1000

#define TIMER_TAG_CHECK_NUM 0
#define TIMER_TAG_CHECK_TIMEOUT_MS 100

int total_interrupts;

/*
struct timer_interrupt {
    volatile boolean fired;
    portMUX_TYPE mutex;
    hw_timer_t* timer;
}

void create_timer_interrupt(struct timer_interrupt* interrupt, uint8_t
        hw_timer_num, int timeout_ms, void (*callback)(void)) {
    timer_interrupt->fired = false;
    timer_interrupt->mutex = portMUX_INITIALIZER_UNLOCKED;
    timer_interrupt->timer = timerBegin(hw_timer_num, TIMER_INC_US, TIMER_COUNT_UP);
    timerAttachInterrupt(timer_interrupt->timer, callback, TIMER_EDGE_TRIGGER);
    timerAlarmWrite(timer_interrupt->timer, timeout_ms * US_TO_MS, TIMER_AUTORELOAD);
    timerAlarmEnable(timer_interrupt->timer);
}
*/

hw_timer_t *timer_tag_check = NULL;
volatile boolean i_t_tag_check;
portMUX_TYPE i_t_tag_check_mutex = portMUX_INITIALIZER_UNLOCKED;
volatile boolean i_g_button;
portMUX_TYPE i_g_button_mutex = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR i_t_tag_check_cb() {
    portENTER_CRITICAL_ISR(&i_t_tag_check_mutex);
    i_t_tag_check = true;
    portEXIT_CRITICAL_ISR(&i_t_tag_check_mutex);
}

void IRAM_ATTR i_g_button_cb() {
    portENTER_CRITICAL_ISR(&i_g_button_mutex);
    i_g_button = true;
    portEXIT_CRITICAL_ISR(&i_g_button_mutex);
}

void interrupts_setup() {
    timer_tag_check = timerBegin(TIMER_TAG_CHECK_NUM, TIMER_INC_US, TIMER_COUNT_UP);
    timerAttachInterrupt(timer_tag_check, &i_t_tag_check_cb, TIMER_EDGE_TRIGGER);
    timerAlarmWrite(timer_tag_check, TIMER_TAG_CHECK_TIMEOUT_MS * US_TO_MS, TIMER_AUTORELOAD);
    timerAlarmEnable(timer_tag_check);

    pinMode(PIN_BUT, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_BUT), i_g_button_cb, FALLING);
}

void setup(void) {
    randomSeed(micros());
    pinMode(PIN_OUT_TAG_READ_STATUS_LED, OUTPUT);
    pinMode(PIN_LED2, OUTPUT);
    debug_setup();

    radio_setup();
    nfc_setup();

    interrupts_setup();
    log("ready");
}

const char* mqtt_server = "mqtt.eclipse.org";
int mqtt_port = 1883;

WiFiClient mqtt_wifi_client;
PubSubClient mqtt_client(mqtt_wifi_client);

void mqtt_setup() {
    mqtt_client.setServer(mqtt_server, mqtt_port);
}

#define MQTT_CLIENT_ID_LEN 10
char mqtt_client_id[MQTT_CLIENT_ID_LEN];
boolean mqtt_try_connect() {
    log("mqtt", "connecting...");
    utils_rand_str(mqtt_client_id, MQTT_CLIENT_ID_LEN - 1);
    log("mqtt", mqtt_client_id);
    if (mqtt_client.connect(mqtt_client_id)) {
        log("mqtt", "connected");
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
    log("mqtt", "disconnecting...");
    mqtt_client.disconnect();
}

void loop(void) {
    if (i_t_tag_check) {
        log("checking for tag presence");
        portENTER_CRITICAL(&i_t_tag_check_mutex);
        i_t_tag_check = false;
        portEXIT_CRITICAL(&i_t_tag_check_mutex);

        // don't queue any more scans while we're busy
        //timerAlarmDisable(timer_scan);

        digitalWrite(PIN_OUT_TAG_READ_STATUS_LED, HIGH);
        if (nfc_tag_present()) {
            NfcTag tag;
            nfc_read(&tag);
        }
        digitalWrite(PIN_OUT_TAG_READ_STATUS_LED, LOW);
    } else if (i_g_button) {
        portENTER_CRITICAL(&i_g_button_mutex);
        i_g_button = false;
        portEXIT_CRITICAL(&i_g_button_mutex);
    }
}
