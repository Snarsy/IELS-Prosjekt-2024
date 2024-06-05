#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <Adafruit_APDS9960.h>

//Adafruit verdier
Adafruit_APDS9960 apds;

U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI display(U8G2_R0, /* clock=*/ 19, /* data=*/ 23, /* cs=*/ 18, /* dc=*/ 17, /* reset=*/ 27);

void setup() {
    Serial.begin(9600);

    //Bevegelsesføler
    if(!apds.begin()){
      Serial.println("failed to initialize device! Please check your wiring.");
    }
    else Serial.println("Device initialized!");
    //Iverksett apds
    apds.enableProximity(true);
    apds.enableGesture(true);

    display.begin();
    display.setFont(u8g_font_6x10);

}

//Batteri-variabler
int batteryCharge = 50;
int maxCharge = 50;
int transactionCaseNumber = 0;

void skjerm(){
  display.firstPage();
  do{
    display.setCursor(10, 45);
    display.print("lol");
    }while(display.nextPage());
}

void batterygestures(){
    uint8_t gesture = apds.readGesture();

    if(gesture == APDS9960_DOWN){
        batteryCharge -= 10;
        Serial.println("Down!");    

    }

  if(gesture == APDS9960_UP){
    if(maxCharge == batteryCharge){
        Serial.println("UP!");    
        delay(3000);
    } else{
      batteryCharge += 10; 
        Serial.println("UP!");    
    } 
  } 

  if(gesture == APDS9960_LEFT){
    transactionCaseNumber = 1;
    Serial.println("LEFT!");    
  } 

  if(gesture == APDS9960_RIGHT){
    transactionCaseNumber = 2;
    Serial.println("RIGHT!");    
  } 
}

void loop() {
  batterygestures();
  //skjerm();
}