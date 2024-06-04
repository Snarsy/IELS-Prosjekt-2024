
#include <IRremoteESP8266.h>
#include <IRsend.h>


const uint16_t IRPin = 32;  // ESP32 pin GPIO 32

IRsend irsend(IRPin);

void setup() {
  
  Serial.begin(115200);

  //Start ir sending
  irsend.begin();
  Serial.println("IR LED enabled");
}

void loop() {
  irsend.sendNEC(0x22222222, 32); //Venter 100ms hver gang. På arduino bruker vi millis, men her bruker vi delay. Det går fint med begge deler.
  
  delay(100);
}
