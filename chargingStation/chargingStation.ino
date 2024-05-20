#include "Adafruit_APDS9960.h"

Adafruit_APDS9960 apds;
int transactionCaseNumber = 0;
//batterihelse
int receivedBatteryHealth = 60;  
int newbatteryhealth = receivedBatteryHealth;
int boughtbatteryhealth = 0;

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

void receiveBatteryHealth(){
//Finne ut hvordan vi skal gjøre dette
}

void batterygestures(){
  //read a gesture from the device
  uint8_t gesture = apds.readGesture();
  if(gesture == APDS9960_DOWN){
      Serial.println("Price going down");
      boughtbatteryhealth -= 10;
      newbatteryhealth = receivedBatteryHealth + boughtbatteryhealth;
      Serial.print("New battery health =");
      Serial.println(newbatteryhealth);
  }
  if(gesture == APDS9960_UP){
      Serial.println("Price going up");
      boughtbatteryhealth += 10;
      newbatteryhealth = receivedBatteryHealth + boughtbatteryhealth;
      Serial.print("New battery health =");
      Serial.println(newbatteryhealth);        
  } 
  if(gesture == APDS9960_LEFT){
    transactionCaseNumber = 1;
  } 
  if(gesture == APDS9960_RIGHT){
    transactionCaseNumber = 2;
} 
}

void doyouwanttocancel(){
  Serial.println("Do you want to cancel the transaction?");
  Serial.println("Left - No     Right - Yes");
  uint8_t gesture = apds.readGesture();
  if (gesture == APDS9960_LEFT){
    Serial.println("Back to menu");
    transactionCaseNumber = 0;
  }
  if (gesture == APDS9960_RIGHT){
    Serial.println("Goodbye");
  }
}

void doyouwanttobuy(){
  Serial.print("Do you want to buy ");
  Serial.print(boughtbatteryhealth);
  Serial.println("% ?");
  Serial.println("Left - No     Right - Yes");
  uint8_t gesture = apds.readGesture();
  if (gesture == APDS9960_LEFT){
    Serial.println("Back to menu");
    transactionCaseNumber = 0;
  }
  if (gesture == APDS9960_RIGHT){
    Serial.print("Bought ");
    Serial.print(boughtbatteryhealth);
    Serial.println("%");
    Serial.print("Batteryhealth: ");
    Serial.print(newbatteryhealth);
    Serial.println("%");
  }
}


// the loop function runs over and over again forever
void loop() {
transactionCase();
}

void transactionCase(){
switch(transactionCaseNumber){
  case 0:
  batterygestures();

  break;

  case 1:
  //Cancel transaction?
  doyouwanttocancel();
  break;

  case 2:
  // Purchase transaction?
  doyouwanttobuy();
  break;

  case 3:
  Serial.println("Goodbye");
  delay(1000);
  break;
}
}