#include <IRremote.h>

IRsend ir;

const int IRPIN = 35;
const int hexForBom = 0x45678912;
const int IR_delay = 100;

long lastSentIR = 0;

void setup(){
    Serial.begin(9600);

    ir.begin(IRPIN);
}

void IRSender(){
    long now = millis();
    if(now - lastSentIR > IR_delay){
        lastSentIR = now;
        ir.sendNEC(hexForBom, 32);
    }
}

void loop(){
    IRSender();
}