#include <Wire.h>
#include <Arduino.h>
#include <SPI.h>
#include <U8glib.h>

int wireCase;
int batteryCharge = 0;
int skjerm = 1;

U8GLIB_SSD1306_128X64 u8g(12, 11, 10, 9, 8);

void receiveEvent(int howMany)
{
  while(1 < Wire.available()) // loop through all but the last
  {
    wireCase = Wire.read(); // receive byte as a character
    Serial.print(wireCase);         // print the character
  }
}

void wireNumber(){
    switch (wireCase)
    {

    case 0:
        break;

    //MINKER BATTERILADNING // NED
    case 1:
        batteryCharge -= 10;
        wireCase = 0;
        break;

    //ØKER BATTERILADING // OPP
    case 2:
        batteryCharge += 10;
        wireCase = 0;
        break;
    
    //MAKS LADING NÅDD
    case 3:
        skjerm = 10;
        wireCase = 0;
        break;
    
    //TILBAKE / NEI / AVBRYT // VENSTRE
    case 11:
        if(skjerm == 0){
            skjerm = 1;
        }
        else{
        skjerm -= 1;
        }
        wireCase = 0;
        break;

    //VIDERE / JA / KJØP // HØYRE
    case 12:
        skjerm += 1;
        wireCase = 0;
        break;

    default:
        break;
    }
}


void skjermValg(){
    switch (skjerm)
    {
    case 0:
        //Avbryt
        u8g.firstPage();
        do{
            u8g.drawStr( 35, 35, "Avbryt");
            }while(u8g.nextPage());

        break;
    
    
    
    case 1:
        //Hjemskjerm
        u8g.firstPage();
        do{
            u8g.drawStr( 35, 10, "Ladeprosent");
            if(batteryCharge == 0){
                u8g.drawStr( 60, 30, "0");
            }
            if(batteryCharge == 10){
                u8g.drawStr( 60, 30, "10");
            }
            if(batteryCharge == 20){
                u8g.drawStr( 60, 30, "20");
            }
            if(batteryCharge == 30){
                u8g.drawStr( 60, 30, "30");
            }
            if(batteryCharge == 40){
                u8g.drawStr( 60, 30, "40");
            }
            if(batteryCharge == 50){
                u8g.drawStr( 60, 30, "50");    
            }
            if(batteryCharge == 60){
                u8g.drawStr( 60, 30, "60");    
            }
            if(batteryCharge == 70){
                u8g.drawStr( 60, 30, "70");
            }
            if(batteryCharge == 80){
                u8g.drawStr( 60, 30, "80");
            }
            u8g.drawStr(7, 60, "Avbryt");
            u8g.drawStr(100, 60, "Betal");
            }while(u8g.nextPage());
        break;
        
    case 2: 
        //Kjøpeskjerm
        u8g.firstPage();
        do{
            u8g.drawStr( 35, 22, "Godkjenn");
            u8g.drawStr( 35, 35, "betaling");
            u8g.drawStr( 7, 60, "Nei");
            u8g.drawStr( 100, 60, "Ja");
            }while(u8g.nextPage());

        break;
        
    case 3:
        //Hadeskjerm
        u8g.firstPage();
        do{
            u8g.drawStr( 35, 10, "Lading");
            u8g.drawStr( 35, 35, "ferdig");
            u8g.drawStr( 35, 60, "Hade:)");
            }while(u8g.nextPage());
        delay(3000);
        skjerm = 1;

        break;

    
    //FOR HØY STRØM
    case 10:
        //Skriv maks ladning nådd
        u8g.firstPage();
        do{
            u8g.drawStr( 35, 10, "Maks");
            u8g.drawStr( 60, 10, "lading");
            
            }while(u8g.nextPage());

        delay(3000);
        skjerm = 1;
        break;

    default:
        break;
    }
}


void setup(){

    Wire.begin(4);
    Wire.onReceive(receiveEvent); 
    u8g.begin();
    u8g.setFont(u8g_font_unifont);
}

void loop(){
    wireNumber();
    skjermValg();
}


/*#include <Arduino.h>
#include <SPI.h>
#include "U8glib.h" // include the universal graphcs library

//U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI display(U8G2_R0, /* clock=/ 13, / data=/ 11, / cs=/ 10, / dc=/ 9, / reset=*/ 8);
/*int batteryCharge;
 
U8GLIB_SSD1306_128X64 u8g(12, 11, 10, 9, 8);    // initialise a u8g display object
//clk = 12, mosi = 11, cs = 10, dc = 9, res = 8.  på arduino

//U8GLIB_SSD1306_128X64 u8g(19, 23, 18, 17, 27);    // initialise a u8g display object
//clk = 19, mosi = 23, cs = 18, dc = 17, res = 27. på esp32

void setup() {
  u8g.begin();
  u8g.setFont(u8g_font_6x10);  //unifont
}

void skjerm(){
  u8g.firstPage();
  do{
    u8g.drawStr( 35, 10, "Ladeprosent");
    u8g.drawStr( 60, 30, "batteryCharge");
    u8g.drawStr(7, 60, "Avbryt");
    u8g.drawStr(100, 60, "Kjop");

    }while(u8g.nextPage());
  
}

void loop() {
  skjerm();
}*/
//Fjern neopixel