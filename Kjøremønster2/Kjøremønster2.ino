#include <Zumo32U4.h> //Initialiserer zumo komponentene og laster inn Turnsensor som lar bilen rotere 90 grader
#include <IRremote.h>
Zumo32U4OLED display;
Zumo32U4Buzzer buzzer;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonC buttonC;
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;
Zumo32U4IRPulses irSensors;
Zumo32U4IMU imu;
#include "TurnSensor.h"

//For ir sender
#include "Zumo32u4IRsender.h"
#define DEVICE_ID 35
Zumo32U4IRsender ZumoIrSender(DEVICE_ID, RIGHT_IR);

#define PRINT_DEGUB_INFO 0 // Set to 1 to enable debug info

#define SUBCARIER_PERIOD 420 // The default frequency is 16000000 / (420 + 1) = 38.005 kHz
#define IR_BRIGHTNESS 50     // 0-400, 0 is off, 400 is max brightness

#define RIGHT_IR 0
#define LEFT_IR 1

const int IRPin = A4;

IRrecv IR(IRPin);

//Variabler for kjøremonsteret
int prevcase;
int driveOverNum = 0;
unsigned long prevmillis = 0;
int caseNum = 1;
int linelength = 300;

//Variabler for gasStation 
int chargePrevMillis;
int howMuchGas = 1;

//Variabler for garasjen
int parkingAvailable = 0;//Har nummeret 10 fram til bilen får ledig plass.


//Bompenger
unsigned long BompreviousMillis = 0; // Store the last time the IR sensor was triggered
const long Bominterval = 10000; // 10 seconds interval


void driveOverLine(){
    switch (driveOverNum){
        case 0:
            prevmillis = millis();
            driveOverNum = 1;
            break;
        case 1:
            motors.setSpeeds(100,100);
            if(millis()-prevmillis>linelength){
                caseNum = prevcase;
                driveOverNum = 0;
                motors.setSpeeds(0,0);
            }
            break;
    }
}

void irDecodeGarasje(){
    if(buttonC.isPressed()) parkingAvailable = 1; //Er til slik at det er mulig å teste garasjen uten diode. Fjerne ved endelig versjon.
    
    if (IR.decode())
    {
        Serial.println(IR.decodedIRData.decodedRawData);
        if (IR.decodedIRData.decodedRawData == 2592268650)  parkingAvailable = 1; //Første verdi
        if (IR.decodedIRData.decodedRawData == 510274632)   parkingAvailable = 2; //Andre verdi
        if (IR.decodedIRData.decodedRawData == 1277849113 ) parkingAvailable = 3; //Tredje verdi
        if (IR.decodedIRData.decodedRawData == 2163717077 ) parkingAvailable = 4; //Fjerde verdi
        if (IR.decodedIRData.decodedRawData == 2227349217 ) parkingAvailable = 5; //Femte verdi
        IR.resume();
    }
}
<<<<<<< Updated upstream
//void irDecodeBensin(){
  //  if (IR.decode())
    //  Serial.println(IR.decodeIRData.decodedRawData);
      //  if (IR.decodedIRData.decodedRawData == ?)     howMuchGas = 1; //10
        //if (IR.decodedIRData.decodedRawData == ?)     howMuchGas = 2; //20
        //if (IR.decodedIRData.decodedRawData == ?)     howMuchGas = 3; //30
        //if (IR.decodedIRData.decodedRawData == ?)     howMuchGas = 4; //40
        //if (IR.decodedIRData.decodedRawData == ?)     howMuchGas = 5; //50   
        //if (IR.decodedIRData.decodedRawData == ?)     howMuchGas = 6; //60
        //if (IR.decodedIRData.decodedRawData == ?)     howMuchGas = 7; //70
        //if (IR.decodedIRData.decodedRawData == ?)     howMuchGas = 8; //80
        ////if (IR.decodedIRData.decodedRawData == ?)     howMuchGas = 9; //90
        ////if (IR.decodedIRData.decodedRawData == ?)     howMuchGas = 10; //100
        
    //}  
//}

int destination = 3;
int currentPosition = 2;
=======
int howMuchGas = 1;
/*void irDecodeBensin(){
    if (IR.decode()){
        Serial.println(IR.decodeIRData.decodedRawData);
        if (IR.decodedIRData.decodedRawData == 2290649224)     howMuchGas = 1; //10
        if (IR.decodedIRData.decodedRawData == 1216907400)     howMuchGas = 2; //20
        if (IR.decodedIRData.decodedRawData == 1149798536)     howMuchGas = 3; //30
        if (IR.decodedIRData.decodedRawData == 1145604232)     howMuchGas = 4; //40
        if (IR.decodedIRData.decodedRawData == 1145342088)     howMuchGas = 5; //50   
        if (IR.decodedIRData.decodedRawData == 1145324680)     howMuchGas = 6; //60
        if (IR.decodedIRData.decodedRawData == 1145324616)     howMuchGas = 7; //70
        if (IR.decodedIRData.decodedRawData == 1145324612)     howMuchGas = 8; //80
        IR.resume()   
    }  
}*/

int destination = 2;
int currentPosition = 1;
>>>>>>> Stashed changes
bool clockWise = 1;

int caseNumGarage = 0;
int currentPosGarage = 0;
bool doDrive = 0;
int charged = 0;
bool haveturned = 0;

void garage(){
    followLinemaxSpeed = 200;
    switch (caseNumGarage){
        case 0://Denne casen får zumo'n til å kjøre over linjen for så å rotere utifra hvilken retning den kommer fra(clockwise). Deretter kjører den frem, stopper og venter på ir signal.
            if(!haveturned){//Her må det være mindre enn samme verdi som 81.
                if(clockWise){turndeg(-90);};
                if(!clockWise){turndeg(90);};
                prevcase = caseNum;
                caseNum = 0;
                linelength = 50;// Lengden bilen kjører over må være samme tall som i linje 76. Dette er slik at den ikke kjører lengre enn den skal.
                haveturned = !haveturned;
                break;
            }
            motors.setSpeeds(0,0);
            linelength = 400;//Hvor mange millisekunder zumo'n skal kjøre over en linje
            irDecodeGarasje();
            if(millis()-prevmillis>1000){
                ZumoIrSender.send(1);
                prevmillis = millis();
            }
            if(parkingAvailable != 0){
                caseNumGarage = 1;
            }
            break;
        case 1:// Her er det en rekke tilfeller. Sjekker om bilen er ved linje hvor den vil kjøre over. Deretter kjører den inn eller over krysset.
            driveLinePID();
            if(aboveAll() && currentPosGarage == (parkingAvailable+1)){//Hvis den treffer et kryss og er på riktig plass i garasjen vil den snu 180 grader og bytte til case 2 hvor den står i ro.
                prevmillis = millis();
                if(parkingAvailable == 1){// Hvis den ikke får parkere ved at det ikke er plass vil den kjøre over linjen og bytte til case 3.
                    caseNumGarage = 3;
                    prevcase = caseNum;
                    caseNum = 0;
                    break;
                }
                turndeg(175);
                caseNumGarage = 2;
                break;
            }
            if(aboveLeft()){//Denne skjer førrst ved at uansett hvilket kryss det er så vil zumo'n kjøre over ved hjelp av case 0 i switch(caseNum).
                currentPosGarage = currentPosGarage + 1;
                prevcase = caseNum;
                caseNum = 0;
                break;
            }
            if(currentPosGarage == parkingAvailable){//Hvis den er på riktig plass vil zumo'n rotere inn for så å kjøre videre med pid kjøring.
                turndeg(-90);
                currentPosGarage = currentPosGarage + 1;
                break;
            }
            if(currentPosGarage == 9){//Denne kommer i bruk når bilen kjører ut. Dette skjer etter case 2.
                turndeg(90);
                caseNum = 1;
                currentPosition = 2;
                haveturned = 0;
            }
            break;
        case 2:
            if(millis()-prevmillis<3000 && currentPosGarage == (parkingAvailable+1)){
                motors.setSpeeds(0,0);
            }
            else{
                driveLinePID();
                if(aboveAll()){
                    prevcase = caseNum;
                    caseNum = 0;
                    currentPosGarage = currentPosGarage + 1;
                    break;
                }
                if(currentPosGarage == (parkingAvailable+2)){
                    caseNumGarage = 1;
                    turndeg(-90);
                }
            }
            break;
        case 3:
            if((millis()-prevmillis)<(linelength+400)){
                turndeg(-90);
                caseNumGarage = 1;
                currentPosGarage = 8;
            }
            break;
    }
}

void gasStation(){
    followLinemaxSpeed = 200;
    if(!haveturned){//Her må det være mindre enn samme verdi som 81.
        if(clockWise) turndeg(90);
        if(!clockWise) turndeg(-90);
        doDrive = 1;
        haveturned = !haveturned;
    }
    if(doDrive == 1){
        driveLinePID();
    }
    
    //irDecodeBensin();   //trenger ikke denne?
    chargePrevMillis = millis();
    if (aboveAll() && charged != 1){
        motors.setSpeeds(0,0);
        doDrive = 0;
        if(clockWise) turndeg(90);
        if(!clockWise) turndeg(-90);
        //irDecodeBensin();
        display.clear();
        display.gotoXY(0,0);
        
        if(howMuchGas == 0){
            display.print("Ingen lading utført");
        }

        if(howMuchGas == 1){
            display.print("Lading: +10%");
        }

        if(howMuchGas == 2){
            display.print("Lading: +20%");
        }

        if(howMuchGas == 3){
            display.print("Lading: +30%");
        }

        if(howMuchGas == 4){
            display.print("Lading: +40%");
        }

        if(howMuchGas == 5){
            display.print("Lading: +50%");
        }

        if(howMuchGas == 6){
            display.print("Lading: +60%");
        }

        if(howMuchGas == 7){
            display.print("Lading: +70%");
        }

        if(howMuchGas == 8){
            display.print("Lading: +80%");
        }
        
        if (millis() - chargePrevMillis > 3000){
            display.clear();
            prevcase = caseNum;
            caseNum = 0;
            charged = 1;
            if(clockWise) turndeg(-90);
            if(!clockWise) turndeg(90);
            doDrive = 1;
        }
    }



    if(aboveAll() && charged == 1){
        prevcase = caseNum;
        caseNum = 0;
        if(clockWise) turndeg(90);
        if(!clockWise) turndeg(-90);
        doDrive = 1;
    }
}

int nabocounter = 0;
int husnummer = 2;

void nabolag(){
    if(!haveturned){
        if(clockWise) turndeg(90);
        if(!clockWise) turndeg(-90);
        haveturned = !haveturned;
        doDrive = 1;
    }
    if(doDrive == 1){
        driveLinePID();
    }
    if(nabocounter == husnummer || nabocounter == (husnummer+4)){
        if(clockWise) turndeg(90);
        if(!clockWise) turndeg(-90);
        nabocounter = nabocounter + 1;
    }
    if(aboveLeft() || aboveRight()){
        nabocounter = nabocounter + 1;
        prevcase = caseNum;
        caseNum = 0;
    }
    if(nabocounter == (husnummer+2)){
        turndeg(170);
        nabocounter = nabocounter + 1;
        doDrive = 0;
        prevmillis = millis();
    }
    if((millis()-prevmillis)>=5000){
        doDrive = 1;
    }
    if(nabocounter == 8){
        if(clockWise) turndeg(90);
        if(!clockWise) turndeg(-90);
        caseNum = 1;
        nabocounter = 0;
    }
}

void driving(){
    switch (caseNum){           //Hovedcase for bilkjøringen. Kan evt legges inn i en void hvis dette ser bedre ut.
            case 0:                 //driveoverline vil kjøre over en linje og returnere til det den gjorde før. Dette gjør at man kan kjøre over kryss og fortsette videre i koden. Husk prevcase = casenum før man setter casenum = 0.
                driveOverLine();
                break;
            case 1:                 //Generellt kjørecase. Vil kjøre over linjene til den treffer ønsket posisjon
                driveLinePID();
                if(aboveAll()){     //Treffer bilen et kryss vil den stoppe og kjøre over for så å oppdatere plasseringen.
                    prevcase = caseNum;
                    caseNum = 0;
                    if(clockWise == 1){
                        currentPosition = currentPosition + 1;
                    }
                    else{
                        currentPosition = currentPosition - 1;
                    }
                    break;
                }
                if(currentPosition == destination){//Hvis bilen er på riktig plass vil den rotere 90 grader og bytte til kjøremønsteret for destinasjonen.
                    prevmillis = millis();
                    if(destination == 1){//I dette tilfellet er destination = 3, garasjen.
                        caseNum = 2;
                    }
                    else if(destination == 2){
                        caseNum = 3;
                    }
                    else if(destination == 3){
                        caseNum = 4;
                    }
                }
                break;
            case 2:
                garage();
                break;
            case 3:
                gasStation();
                break;
            case 4:
                nabolag();
                break;
        }
}    


void setup(){
    IR.begin(IRPin, ENABLE_LED_FEEDBACK);
    lineSensors.initFiveSensors();
    Serial.begin(115200);
    turnSensorSetup();
}

void loop(){
    driving();
    tollGate();
}

void tollGate(){ //Tar imot bompenger, denne må være bare om det er dieselbil
    unsigned long currentMillis = millis(); 
    
    if (IR.decode()){
        if (IR.decodedIRData.decodedRawData == 1217527807){
            if (BompreviousMillis == 0 || currentMillis - BompreviousMillis >= Bominterval) {
                BompreviousMillis = currentMillis;

                //Spiller G4
                buzzer.playFrequency(392, 250, 15);
            }
        }
    }
}