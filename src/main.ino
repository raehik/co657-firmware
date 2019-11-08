#include "config.h"
#include "wifi.h"
#include "mqtt.h"
#include "nfc.h"
#include "msg.h"

#define TAG_CHECK_MS 1000 // TODO 50-500ms
#define INACTIVITY_TIMEOUT_SEC 5 // TODO 5-30m

typedef enum {
    STATE_POWERON,
    STATE_READY,
    STATE_READ,
    STATE_IDLE,
    STATE_SLEEP,
    STATE_SEND,
    STATE_TAGCHECK,
} State ;

State state;
long now;
long activity_last;
long tag_check_last;
NfcTag nfc_tag;

void setup(void) {
    randomSeed(micros());
    pinMode(PIN_OUT_TAG_READ_STATUS_LED, OUTPUT);
    digitalWrite(PIN_OUT_TAG_READ_STATUS_LED, LOW);
    pinMode(PIN_OUT_WIFI_STATUS_LED, OUTPUT);
    pinMode(PIN_BUT, INPUT_PULLUP);

    debug_setup();
    board_general_setup();
    wifi_setup();
    mqtt_setup();
    nfc_setup();

    now = millis();
    activity_last = now;
    tag_check_last = now;
}

void board_general_setup() {
    btStop();
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
        case STATE_SEND:
            state_send();
            break;
        default:
            log("error", "got placed into an unhandled state");
    }
}

void set_state(State next_state) {
    state = next_state;
}

// helper function for state transitions which do nothing
void statetrans_simple(State next_state) {
    log("state", "x -> y (unknown)");
    set_state(next_state);
}

// start state and no transitions to, so set self up
void state_poweron(void) {
    log("state", "STATE_POWERON");
    wifi_on();
    wifi_connect();

    mqtt_connect();
    msg_send_status();
    msg_recv_tracked_item_states();
    mqtt_disconnect();

    statetrans_poweron_ready();
}

void statetrans_poweron_ready(void) {
    log("state", "POWERON -> READY");
    wifi_disconnect();
    wifi_off();
    set_state(STATE_READY);
}

boolean timeout_expired(long event_last, long event_cycle_ms) {
    return now - event_last > event_cycle_ms;
}

void state_ready(void) {
    //log("state", "READY");
    now = millis();
    if (timeout_expired(tag_check_last, TAG_CHECK_MS)) {
        tag_check_last = now;
        statetrans_simple(STATE_TAGCHECK);
    } else if (timeout_expired(activity_last, INACTIVITY_TIMEOUT_SEC * 1000)) {
        statetrans_simple(STATE_IDLE);
    // TODO TMP FUN BUTTON CASE
    } else if (digitalRead(PIN_BUT) == LOW) {
        wifi_on();
        wifi_connect();
        mqtt_connect();
        mqtt_send_test_msg("button pressed");
        mqtt_disconnect();
        wifi_disconnect();
        wifi_off();

        delay(1000);

        now = millis();
        activity_last = now;
    }
}

void state_tagcheck(void) {
    log("state", "TAGCHECK");
    if (nfc_tag_present()) {
        statetrans_simple(STATE_READ);
    } else {
        statetrans_simple(STATE_READY);
    }
}

char tmp_msgs[10][20];
int tmp_msgs_i = 0;
void state_read(void) {
    log("state", "READ");
    digitalWrite(PIN_OUT_TAG_READ_STATUS_LED, HIGH);
    nfc_tag_read(&nfc_tag);
    digitalWrite(PIN_OUT_TAG_READ_STATUS_LED, LOW);
    Serial.println(nfc_tag.getTagType());
    log("nfc", nfc_tag.getUidString().c_str());
    //tmp_msgs[tmp_msgs_i] = nfc_tag.getUidString().c_str();
    strcpy(tmp_msgs[tmp_msgs_i], nfc_tag.getUidString().c_str());
    tmp_msgs_i++;
    delay(500);
    activity_last = now;
    statetrans_simple(STATE_READY);
}

void statetrans_idle_send(void) {
    log("state", "IDLE -> SEND");
    wifi_on();
    wifi_connect();
    mqtt_connect();
    set_state(STATE_SEND);
}

boolean queued_msgs_present() {
    return tmp_msgs_i > 0;
}

void state_idle(void) {
    log("state", "IDLE");
    if (queued_msgs_present()) {
        statetrans_idle_send();
    } else {
        statetrans_simple(STATE_SLEEP);
    }
}

void statetrans_send_sleep(void) {
    mqtt_disconnect();
    wifi_disconnect();
    wifi_off();
    set_state(STATE_SLEEP);
}

void state_send(void) {
    log("state", "SEND");
    for (int i = 0; i < tmp_msgs_i; i++) {
        mqtt_send_test_msg(tmp_msgs[i]);
    }
    tmp_msgs_i = 0;
    statetrans_send_sleep();
}

void statetrans_sleep_ready(void) {
    now = millis();
    activity_last = now;
    set_state(STATE_READY);
}

void state_sleep(void) {
    log("(fake sleeping: waiting a bit before going to ready again)");
    delay(5000);
    statetrans_sleep_ready();
}
