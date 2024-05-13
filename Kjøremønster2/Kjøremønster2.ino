#include <Zumo32U4.h> //Initialiserer zumo komponentene og laster inn Turnsensor som lar bilen rotere 90 grader
#include <IRremote.h>
Zumo32U4OLED display;
Zumo32U4Buzzer buzzer;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
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
void driveOverLine(int line){
    switch (driveOverNum){
        case 0:
            prevmillis = millis();
            driveOverNum = 1;
            break;
        case 1:
            motors.setSpeeds(100,100);
            if(millis()-prevmillis>line){
                caseNum = prevcase;
                driveOverNum = 0;
                motors.setSpeeds(0,0);
            }
            break;
    }
}

int parkingAvailable = 11;//Har nummeret 10 fram til bilen får ledig plass.
void irDecode(){
    if (IR.decode())
    {
        Serial.println(IR.decodedIRData.decodedRawData);
        if (IR.decodedIRData.decodedRawData == 2592268650)  parkingAvailable = 0; //Første verdi
        if (IR.decodedIRData.decodedRawData == 510274632)   parkingAvailable = 1; //Andre verdi
        if (IR.decodedIRData.decodedRawData == 1277849113 ) parkingAvailable = 2; //Tredje verdi
        if (IR.decodedIRData.decodedRawData == 2163717077 ) parkingAvailable = 3; //Fjerde verdi
        if (IR.decodedIRData.decodedRawData == 2227349217 ) parkingAvailable = 4; //Femte verdi
        IR.resume();
    }
}

int caseNumGarage = 1;
int currentPosGarage = 0;
void garage(){
    followLinemaxSpeed = 150;
    switch (caseNumGarage){
        case 0:
            driveOverLine(500);
            break;
        case 1:
            irDecode();
            if(parkingAvailable!=10){
              caseNumGarage = 2;
            }
        break;
        case 2:
            driveLinePID();
            if(aboveLeft()){
                if(parkingAvailable == 0){
                    turndeg(-90);
                    caseNum = 1;
                }
            }
            break;
    }
}

void setup(){
    IR.begin(IRPin, ENABLE_LED_FEEDBACK);
    lineSensors.initFiveSensors();
    Serial.begin(115200);
    turnSensorSetup();
}

int destination = 3;
int currentPosition = 0;
void loop(){
    switch (caseNum){
        case 0:
            driveOverLine(300);
            break;
        case 1:
            driveLinePID();
            if(aboveAll()){
                prevcase = caseNum;
                caseNum = 0;
                currentPosition = currentPosition + 1;
                break;
            }
            if(currentPosition == destination){
                turndeg(90);
                prevmillis = millis();
                caseNum = destination;
                currentPosition = currentPosition + 1;
                }
            break;
        case 3:
            garage();
            break;
    }
    display.clear();
    display.print(caseNum);
}