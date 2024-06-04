#include <Wire.h>

int c;
int batteryCharge;
int skjerm;

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
        skjerm -= 1;
        c = 0;
        break;

    //VIDERE / JA / KJØP // HØYRE
    case 12:
        skjerm += 1;
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

        break;
    
    //FOR HØY STRØM
    case 3:
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
}

void loop(){
    wireNumber();
    batteryCharging();
    skjermValg();
}