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