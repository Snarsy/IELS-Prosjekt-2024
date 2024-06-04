#include <Wire.h>
#include <Arduino.h>
#include <SPI.h>
#include <U8glib.h>

int c;
int batteryCharge;
int skjerm;

U8GLIB_SSD1306_128X64 u8g(12, 11, 10, 9, 8);

void receiveEvent(int howMany)
{
  while(1 < Wire.available()) // loop through all but the last
  {
    c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
}

void wireNumber(){
    switch (c)
    {

    case 0:
        break;

    //MINKER BATTERILADNING // NED
    case 1:
        batteryCharge -= 10;
        c = 0;
        break;

    //ØKER BATTERILADING // OPP
    case 2:
        batteryCharge += 10;
        c = 0;
        break;
    
    //MAKS LADING NÅDD
    case 3:
        skjerm = 10;
        c = 0;
        break;
    
    //TILBAKE / NEI / AVBRYT // VENSTRE
    case 11:
        if(skjerm == 1){
            skjerm = 3;
        }
        else{
        skjerm -= 1;
        }
        c = 0;
        break;

    //VIDERE / JA / KJØP // HØYRE
    case 12:
        if(skjerm == 3){
            skjerm = 1;
        }
        else{
        skjerm += 1;
        }
        c = 0;
        break;

    default:
        break;
    }
}


void skjermValg(){
    switch (skjerm)
    {
    case 1:
        //Kjøpeskjerm
        u8g.firstPage();
        do{
            u8g.drawStr( 1, 22, "Viva Canarias");
            }while(u8g.nextPage());

        break;

    case 2: 
        
    
    //FOR HØY STRØM
    case 10:
        //Skriv maks ladning nådd

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


#include <Arduino.h>
#include <SPI.h>
#include "U8glib.h" // include the universal graphcs library

//U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI display(U8G2_R0, /* clock=/ 13, / data=/ 11, / cs=/ 10, / dc=/ 9, / reset=*/ 8);
int batteryCharge;
 
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
}
//Fjern neopixel