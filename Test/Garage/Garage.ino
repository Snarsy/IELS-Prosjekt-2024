#include <Wire.h>
#include <Zumo32U4.h>
#include <IRremote.h>
Zumo32U4OLED display;
Zumo32U4ButtonA buttonA;
Zumo32U4Buzzer buzzer;
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;
Zumo32U4IRPulses irSensors;
Zumo32U4IMU imu;

#include "TurnSensor.h"

int whereTo = 3;
int gps = 0;
int caseNum = 0;
int caseNumGarage = 0;
bool go = 0;
bool elbil = true;
int ledigplass = 10;
int currentplass = 0;
int driveovertime = 600;
int right = 1;
int followLinemaxSpeed = 200;
int lastError = 0;
int directionCircle = right;
unsigned long prevmillis = 0;

#define PRINT_DEGUB_INFO 0 // Set to 1 to enable debug info

#define SUBCARIER_PERIOD 420 // The default frequency is 16000000 / (420 + 1) = 38.005 kHz
#define IR_BRIGHTNESS 50     // 0-400, 0 is off, 400 is max brightness

#define RIGHT_IR 0
#define LEFT_IR 1

const int IRPin = A4;

IRrecv IR(IRPin);


int irGarageCase = 0;

void driveLinePID()
{ //Linjefølging tatt fra Kevin Pololu. 
    int16_t position = lineSensors.readLine(lineSensorValues); //Leser linjeesensor. 
    int16_t error = position - 2000; //Finnet ut om det er venstre eller høyre som er mer utenfor linjen.
    int16_t speedDifference = error / 1 + 4 * (error - lastError); 

    lastError = error;

    int leftSpeed = followLinemaxSpeed + speedDifference;
    int rightSpeed = followLinemaxSpeed - speedDifference;
    leftSpeed = constrain(leftSpeed, 0, (int16_t)followLinemaxSpeed); //Constrain for å ikke få minus, og ikke over 400 verdi. 
    rightSpeed = constrain(rightSpeed, 0, (int16_t)followLinemaxSpeed);

    motors.setSpeeds(leftSpeed, rightSpeed);
}
void irDecode(){
    if (IR.decode())
    {
        Serial.println(IR.decodedIRData.decodedRawData);
        if (IR.decodedIRData.decodedRawData == 2592268650)  ledigplass = 0; //Første verdi
        if (IR.decodedIRData.decodedRawData == 510274632)   ledigplass = 1; //Andre verdi
        if (IR.decodedIRData.decodedRawData == 1277849113)  ledigplass = 2; //Tredje verdi
        if (IR.decodedIRData.decodedRawData == 2163717077)  ledigplass = 3; //Fjerde verdi
        if (IR.decodedIRData.decodedRawData == 2227349217)  ledigplass = 4; //Femte verdi
        IR.resume();
    }
}
void garage(){
    switch (caseNumGarage){
<<<<<<< Updated upstream:Test/Garage/Garage.ino
        case 0://Bilen kjører til den skal stoppe og kjører hvis den får ir signal.
            if(millis()-prevmillis>driveovertime){
              motors.setSpeeds(0,0);
=======
                case 0://Bilen kjører til den skal stoppe og kjører hvis den får ir signal.
                    if(millis()-prevmillis>driveovertime){
                      motors.setSpeeds(0,0);
                    }
                    irDecode();
                    if(ledigplass!=10){
                      caseNumGarage = 1;
                    }
                break;
                case 1://Bilen kjører til den når riktig plass.
                    followLinemaxSpeed = 150;
                    readSensors();
                    if(aboveLine(0) && aboveLine(1) && (currentplass) == ledigplass){//Hvis den er på riktig plass vil den rotere 90 grader 
                        if(ledigplass == 0){//Er den på første plass så vil den rotere og kjøre ut. Den vil bytte til linjefølger og gå tilbake til hovedcase.
                        turndeg(-87);
                        caseNum = 0;
                        caseNumGarage = 0;
                        break;
                        }
                        caseNumGarage = 3;//Bytter til innkjøringscase 3
                        prevmillis = millis();
                    }
                    else if(aboveLine(0) && aboveLine(1)){//Hvis den ikke er ved plassen vil den bytte til case 2
                        currentplass = currentplass + 1;
                        caseNumGarage = 2;
                        prevmillis = millis();
                    }
                    driveLinePID();
                break;
                case 2://Vil kjøre over krysset og bytte til case 1 når den er kjørt over
                    motors.setSpeeds(100,100);
                    if(millis()-prevmillis>driveovertime){
                        caseNumGarage = 1;
                    }
                break;
                case 3://Innkjøringscase. Kjører inn
                    motors.setSpeeds(100,100);
                    if(millis()-prevmillis>400){
                        motors.setSpeeds(0,0);
                        turndeg(-87);
                        motors.setSpeeds(100,100);
                        prevmillis = millis();
                        caseNumGarage = 4;
                    }
                break;
                case 4:
                readSensors();
                if(aboveLine(0) && aboveLine(1) && aboveLine(2) && aboveLine(3) && aboveLine(4)){
                    motors.setSpeeds(0,0);
                    turndeg(176);
                    caseNumGarage = 5;
                    prevmillis = millis();
                }
                break;
                case 5:
                    if(millis()-prevmillis>5000){
                        driveLinePID();
                        if(aboveLine(0) && aboveLine(1) && aboveLine(2) && aboveLine(3) && aboveLine(4)){
                            turndeg(-87);
                        }
                    }
                break;
>>>>>>> Stashed changes:Garage/Garage.ino
            }
            
            irDecode();
            if(ledigplass!=10){
              caseNumGarage = 1;
            }
        break;
        case 1://Bilen kjører til den når riktig plass.
            followLinemaxSpeed = 150;
            readSensors();
            if(aboveLine(0) && aboveLine(1) && (currentplass) == ledigplass){//Hvis den er på riktig plass vil den rotere 90 grader 
              if(ledigplass == 0){//Er den på første plass så vil den rotere og kjøre ut. Den vil bytte til linjefølger og gå tilbake til hovedcase.
                turndeg(-90);
                caseNum = 0;
                caseNumGarage = 0;
                break;
              }
                caseNumGarage = 3;//Bytter til innkjøringscase 3
                prevmillis = millis();
            }
            else if(aboveLine(0) && aboveLine(1)){//Hvis den ikke er ved plassen vil den bytte til case 2
                currentplass = currentplass + 1;
                caseNumGarage = 2;
                prevmillis = millis();
            }
            driveLinePID();
        break;
        case 2://Vil kjøre over krysset og bytte til case 1 når den er kjørt over
            motors.setSpeeds(100,100);
            if(millis()-prevmillis>driveovertime){
                caseNumGarage = 1;
            }
        break;
        case 3://Innkjøringscase. Kjører inn
            motors.setSpeeds(100,100);
            if(millis()-prevmillis>400){
                motors.setSpeeds(0,0);
                turndeg(-90);
                motors.setSpeeds(100,100);
                prevmillis = millis();
                caseNumGarage = 4;
            }
        break;
        case 4:
        readSensors();
        if(aboveLine(0) && aboveLine(1) && aboveLine(2) && aboveLine(3) && aboveLine(4)){
            turndeg(180);
            prevmillis = millis();
            caseNumGarage = 5;
        }
        break;
        case 5:
            if(millis()-prevmillis>5000){
                driveLinePID();
                if(aboveLine(0) && aboveLine(1) && aboveLine(2) && aboveLine(3) && aboveLine(4)){
                    caseNumGarage = 6;
                    driveovertime = 400;
                    prevmillis = millis();
                    motors.setSpeeds(100,100);
                }
            }
        break;
        case 6:
            if(millis()-prevmillis>300){
                turndeg(-90);
                caseNumGarage = 7;
            }
        break;
        case 7:
            driveLinePID();
            readSensors();
            if(aboveLine(0) && aboveLine(1)){//Hvis den ikke er ved plassen vil den bytte til case 2
                caseNumGarage = 8;
                prevmillis = millis();
                currentplass = currentplass + 1;
            }
            if(currentplass == 5){
                caseNum = 0;
                caseNumGarage = 0;
            }
        break;
        case 8:
            motors.setSpeeds(100,100);
            if(millis()-prevmillis>200){
                caseNumGarage = 7;
            }
        break;
    }
}

void setup(){
    lineSensors.initFiveSensors();
    Serial.begin(9600);
    turnSensorSetup();
    IR.begin(IRPin, ENABLE_LED_FEEDBACK);
}
void loop(){
  irDecode();
    switch(caseNum){
        case 0://Hovedcase. Her kjører bilen med linjefølger og stopper når den når ønskede mål. Ellers kjører den over kryss i case 1.
          followLinemaxSpeed = 200;
          driveLinePID();
          if(aboveLine(0) && aboveLine(1) && aboveLine(2) && aboveLine(3) && aboveLine(4) && directionCircle == right && (gps+1) == whereTo){
              caseNum = 2;
              motors.setSpeeds(100,100);
              prevmillis = millis();
          }
          else if(aboveLine(0) && aboveLine(1) && aboveLine(2) && aboveLine(3) && aboveLine(4) && directionCircle == right){
              gps = gps+1;
              caseNum = 1;
              prevmillis = millis();
          }
        break;

        case 1://Bilen kjører rett fram over krysset.
            motors.setSpeeds(100,100);
            if(millis()-prevmillis>200){
                caseNum = 0;
            }
        break;
        case 2://Bilen stopper etter den har kjørt litt fremmover.
            if(millis()-prevmillis>250){
                motors.setSpeeds(0,0);
                turndeg(90);
                motors.setSpeeds(100,100);
                prevmillis = millis();
                if(whereTo == 3){//Utifra valgt plass(whereTo) så bytter den til neste case.
                    caseNum = 3;
                }
                else if(whereTo == 2){
                    caseNum = 4;
                }
            }
        break;
        case 3://Garasje case.
            garage();
        break;
    }
  }