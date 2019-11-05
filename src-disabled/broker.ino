#include "debug.h"
#include <Wire.h>
#include <WiFi.h>
#include <PN532.h>

//#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define PIN_LED BUILTIN_LED

// Replace with your network credentials
//char* ssid     = "***REMOVED***";
//const char* password = "***REMOVED***";
char* ssid     = "TODO";
const char* password = "TODO";
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void wifi_connect_builtin() {
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

void setup() {
  pinMode(PIN_LED, OUTPUT);
  debug_setup();
  wifi_connect_builtin();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

/*
void loop(){
  digitalWrite(PIN_LED, HIGH);
  log("ping");
  delay(50);
  digitalWrite(PIN_LED, LOW);
  delay(2000);
}
*/

void loop(){
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 50, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(PIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(PIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32-raehik";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
