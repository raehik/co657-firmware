#include "debug.h"

#define SERIAL_RATE 9600

void debug_setup(void) {
    Serial.begin(SERIAL_RATE);
}

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

void log_indicate_wait(void) {
    Serial.print(".");
}

void log_end(void) {
    Serial.print("\n");
}
