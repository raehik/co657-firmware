#include "config.h"
#include "wifi.h"
#include "mqtt.h"
#include "nfc.h"
#include "msg.h"

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

void setup(void) {
    randomSeed(micros());
    pinMode(PIN_OUT_TAG_READ_STATUS_LED, OUTPUT);
    pinMode(PIN_LED2, OUTPUT);

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

long tag_check_ms = 1000; // TODO 50-500ms
long inactivity_timeout_sec = 5; // TODO 5-30m
void state_ready(void) {
    log("state", "READY");
    now = millis();
    if (timeout_expired(tag_check_last, tag_check_ms)) {
        tag_check_last = now;
        statetrans_simple(STATE_TAGCHECK);
    } else if (timeout_expired(activity_last, inactivity_timeout_sec * 1000)) {
        statetrans_simple(STATE_IDLE);
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

NfcTag nfc_tag;
void state_read(void) {
    log("state", "READ");
    activity_last = now;
    log("nfc", "TODO: read tag and place it somewhere");
    nfc_tag_read(&nfc_tag);
    delay(500);
    statetrans_simple(STATE_READY);
}

void state_idle(void) {
    log("state", "IDLE");
    if (queued_msgs_present()) {
        statetrans_idle_send();
    } else {
        statetrans_simple(STATE_SLEEP);
    }
}

void statetrans_idle_send(void) {
    log("state", "IDLE -> SEND");
    wifi_on();
    wifi_connect();
    mqtt_connect();
    set_state(STATE_SEND);
}

void state_send(void) {
    log("state", "SEND");
    log("mqtt", "TODO: sending any queued messages");
    statetrans_simple(STATE_SLEEP);
}

void statetrans_send_sleep(void) {
    mqtt_disconnect();
    wifi_disconnect();
    wifi_off();
    set_state(STATE_SLEEP);
}

void state_sleep(void) {
    log("(fake sleeping: waiting a bit before going to ready again)");
    delay(5000);
    statetrans_simple(STATE_READY);
}

// TODO
boolean queued_msgs_present(void) {
    return false;
}
