#include "Zumo32u4IRsender.h"

#define DEVICE_ID 0x01
#define DIRECTION RIGHT_IR

Zumo32U4IRsender ZumoIrSender(DEVICE_ID, DIRECTION);

void setup() {
  Serial.begin(9600);
}

void loop() {
  ZumoIrSender.send(0x11);
  delay(1000);
}