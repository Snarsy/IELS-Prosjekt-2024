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

#define PRINT_DEGUB_INFO 0 // Set to 1 to enable debug info

#define SUBCARIER_PERIOD 420 // The default frequency is 16000000 / (420 + 1) = 38.005 kHz
#define IR_BRIGHTNESS 50     // 0-400, 0 is off, 400 is max brightness

#define RIGHT_IR 0
#define LEFT_IR 1

const int IRPin = A4;

IRrecv IR(IRPin);

//Alle verdiene vi bruker for kjøremønsteret

int prevcase;
int driveOverNum = 0;
int prevmillis = 0;
int caseNum = 1;
int linelength = 200;
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

int parkingAvailable = 0;//Har nummeret 10 fram til bilen får ledig plass.
void irDecode(){
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

int destination = 2;
int currentPosition = 0;
bool clockWise = 1;

int caseNumGarage = 0;
int currentPosGarage = 0;
void garage(){
    followLinemaxSpeed = 200;
    switch (caseNumGarage){
        case 0://Denne casen får zumo'n til å kjøre over linjen for så å rotere utifra hvilken retning den kommer fra(clockwise). Deretter kjører den frem, stopper og venter på ir signal.
            if(millis()-prevmillis<50){//Her må det være mindre enn samme verdi som 81.
                if(clockWise) turndeg(90);
                if(!clockWise) turndeg(-90);
                prevcase = caseNum;
                caseNum = 0;
                linelength = 50;// Lengden bilen kjører over må være samme tall som i linje 76. Dette er slik at den ikke kjører lengre enn den skal.
                break;
            }
            motors.setSpeeds(0,0);
            linelength = 400;//Hvor mange millisekunder zumo'n skal kjøre over en linje
            irDecode();
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
    }
}

void yx(){
    motors.setSpeeds(0,0);
}

void setup(){
    IR.begin(IRPin, ENABLE_LED_FEEDBACK);
    lineSensors.initFiveSensors();
    Serial.begin(115200);
    turnSensorSetup();
}

void loop(){
    switch (caseNum){           //Hovedcase for bilkjøringen. Kan evt legges inn i en void hvis dette ser bedre ut.
        case 0:                 //driveoverline vil kjøre over en linje og returnere til det den gjorde før. Dette gjør at man kan kjøre over kryss og fortsette videre i koden. Husk prevcase = casenum før man setter casenum = 0.
            driveOverLine();
            break;
        case 1:                 //Generellt kjørecase. Vil kjøre over linjene til den treffer ønsket posisjon
            driveLinePID();
            if(currentPosition>destination && clockWise == 1){//Denne gjør at bilen vil endre rettning for å komme seg raskeere til ønsket posisjon.
                clockWise = 0;
                turnSensorReset();
                turndeg(175);
                break;
            }
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
            }
            break;
        case 2:
            garage();
            break;
        case 3:
            yx();
            break;
    }
}