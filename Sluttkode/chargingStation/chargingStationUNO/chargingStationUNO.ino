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
        if(skjerm == 1 || skjerm == 2){
            skjerm -= 1;
        }
        if(skjerm == 3){
            skjerm = 3;
        }

        wireCase = 0;
        break;

    //VIDERE / JA / KJØP // HØYRE
    case 12:
        if(skjerm == 0 || skjerm == 1 || skjerm == 2){
            skjerm += 1;
        }
        if(skjerm == 3){
            skjerm = 3;
        }   
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
            u8g.drawStr(90, 60, "Betal");
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
    Serial.begin(9600);
    Wire.begin(4);
    Wire.onReceive(receiveEvent); 
    u8g.begin();
    u8g.setFont(u8g_font_6x10);
}

void loop(){
    wireNumber();
    skjermValg();
    Serial.println(wireCase);
}