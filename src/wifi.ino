#include "wifi.h"

#include "config.h"
#include "debug.h"
#include "sensitive.h"
#include <WiFi.h>

void wifi_setup(void) {
    // nothing seems necessary...?
}

void wifi_on(void) {
    // seems unrequired, connect/begin seems to sort it
}

void wifi_connect(void) {
    wifi_connect_builtin();
}

void wifi_disconnect(void) {
    WiFi.disconnect();
}

void wifi_off(void) {
#ifndef DISABLE_PWR_MGMT
    WiFi.mode(WIFI_OFF);
#else
    log("wifi", "power management disabled, ignoring power off command");
#endif
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
