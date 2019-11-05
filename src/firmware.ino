#define PIN_LED_BUILTIN 13

/* --- */

#include <Wire.h>
#include <WiFi.h>
#include <PN532.h>

#define PIN_LED PIN_LED_BUILTIN
#define SERIAL_RATE 115200

// Replace with your network credentials
char* ssid     = "***REMOVED***";
const char* password = "***REMOVED***";

void log(const char* log_group, const char* msg) {
  Serial.print(log_group);
  Serial.print(": ");
  Serial.println(msg);
}

void log(const char* msg) {
  log("main", msg);
}

void log_begin(const char* log_group, const char* msg) {
  Serial.print(log_group);
  Serial.print(": ");
  Serial.print(msg);
  Serial.print("...");
}

void log_indicate_wait() {
  Serial.print(".");
}

void log_end(void) {
 Serial.print("\n");
}

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

void setup_serial() {
  Serial.begin(SERIAL_RATE);
}

void setup() {
  setup_serial();
  wifi_connect_builtin();
  pinMode(PIN_LED, OUTPUT);
}

void loop(){
  digitalWrite(PIN_LED, HIGH);
  log("ping");
  delay(50);
  digitalWrite(PIN_LED, LOW);
  delay(2000);
}
