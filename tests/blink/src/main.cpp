#include <Arduino.h>

// Smoke test exercising the framework end to end: variant pin map
// (LED_BUILTIN), Arduino API entry points (pinMode/digitalWrite/delay),
// and the FreeRTOS-backed loop scheduler.

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
}
