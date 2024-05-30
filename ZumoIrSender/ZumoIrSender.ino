// De nedenfor er til for å initialisere sendingen av ir signal. Dette lar oss ikke sende spesifikke IR signal, men et med koden FFFFFFFF
#include "Zumo32u4IRsender.h"
#define DEVICE_ID 35
uint64_t lastTime = 0;
Zumo32U4IRsender ZumoIrSender(DEVICE_ID, RIGHT_IR);

void setup(){
    Serial.begin(9600);
    delay(3000);
}

void loop(){
    drivelinePID
}