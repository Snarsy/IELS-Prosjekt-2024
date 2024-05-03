#include "Adafruit_APDS9960.h"

Adafruit_APDS9960 apds;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  
  if(!apds.begin()){
    Serial.println("failed to initialize device! Please check your wiring.");
  }
  else Serial.println("Device initialized!");

  //gesture mode will be entered once proximity mode senses something close
  apds.enableProximity(true);
  apds.enableGesture(true);
}

// the loop function runs over and over again forever
void loop() {
  //read a gesture from the device
    uint8_t gesture = apds.readGesture();
    if(gesture == APDS9960_DOWN){
        Serial.println("Price going down");
    }
    if(gesture == APDS9960_UP){
        Serial.println("Price going up");
    } 
    if(gesture == APDS9960_LEFT){
        Serial.println("Cancel transaction - left");
    } 
    if(gesture == APDS9960_RIGHT){
        Serial.println("Buy transaction - right");
    } 
}