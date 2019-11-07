enum state {
    STATE_POWERON,
    STATE_READY,
    STATE_READ,
    STATE_IDLE,
    STATE_SLEEP,
    STATE_SEND,
};

void set_state(state next_state) {
    state = next_state;
}

// helper function for state transitions which do nothing
void statetrans_simple(state next_state) {
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

void state_read(void) {
    log("state", "READ");
    activity_last = now;
    log("nfc", "TODO: read tag and place it somewhere");
    nfc_tag = nfc_tag_read();
    delay(500);
    statetrans_simple(STATE_READY)
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
