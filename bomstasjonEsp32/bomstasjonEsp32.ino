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
  uint16_t address = 0x45678912;
  uint16_t command = 32;
  irsend.sendNEC(address, command); //Send til addresse og command. 

  Serial.println("Sent NEC IR signal");

  
  delay(100);  
}