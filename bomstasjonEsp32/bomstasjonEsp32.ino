#include <IRremoteESP8266.h>
#include <IRsend.h>

// Define the GPIO pin you want to use
const uint16_t kIrLedPin = 17;  // ESP32 pin GPIO 17

IRsend irsend(kIrLedPin);

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Enable the IR LED
  irsend.begin();
  Serial.println("IR LED enabled");
}

void loop() {
  // Send a NEC IR signal (example address and command)
  uint16_t address = 0x45678912;
  uint16_t command = 32;
  irsend.sendNEC(address, command);

  Serial.println("Sent NEC IR signal");

  // Delay before sending the next signal
  delay(100);  // 2 seconds delay
}