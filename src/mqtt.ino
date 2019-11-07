#include "mqtt.h"

#include "debug.h"

#include <WiFi.h>
#include <PubSubClient.h>

const char* mqtt_server = "broker.hivemq.com";
int mqtt_port = 1883;

WiFiClient mqtt_wifi_client;
PubSubClient mqtt_client(mqtt_wifi_client);

void mqtt_setup() {
    log("mqtt", mqtt_server);
    mqtt_client.setServer(mqtt_server, mqtt_port);
}

void mqtt_connect() {
    while (!mqtt_try_connect()) {}
}

void mqtt_disconnect() {
    log("mqtt", "disconnecting...");
    mqtt_client.disconnect();
}

void mqtt_send_test_msg(void) {
    mqtt_client.publish("raehik", "chippy calling in");
}

#define MQTT_CLIENT_ID_LEN 20
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
