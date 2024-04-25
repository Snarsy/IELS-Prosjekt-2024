#include <IRremote.h>
const int IRPIN = 2;

IRsend ir;


void setup(){
    Serial.begin(9600);
    ir.begin(IRPIN);
}

void loop(){
    ir.sendNEC(0x12345678, 32); 
    delay(1000);
}