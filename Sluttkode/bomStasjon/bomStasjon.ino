#include <IRremote.h>

IRsend ir;

const int IRPIN = 2;
const int hexForBom = 0x45678912;
const int IR_delay = 100;

long lastSentIR = 0;

void setup(){
    Serial.begin(9600);
    ir.begin(IRPIN); //Start IR-Pin
}

void IRSender(){
    long now = millis();
    if(now - lastSentIR > IR_delay){ //Sender hvert 100ms. Hadde man ikke hatt delay hadde den sendt 0. 
        lastSentIR = now;
        ir.sendNEC(hexForBom, 32); //Sender IR
    }
}

void loop(){
    IRSender();
}