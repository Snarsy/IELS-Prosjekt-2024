#include <Zumo32U4.h>
Zumo32U4OLED display;
Zumo32U4Buzzer buzzer;
Zumo32U4ButtonA buttonA;
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
Zumo32U4IMU imu;

#include "TurnSensor.h"

int whereTo = 3;
int gps = 0;
int box = 0;
bool go = 0;
bool elbil = true;
int ledigplass = 2;
int currentplass = 0;

const int followLinemaxSpeed = 200;
int lastError = 0;
int direction = 1;
unsigned long prevmillis = 0;

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

void setup(){
    lineSensors.initFiveSensors();
    Serial.begin(115200);
    turnSensorSetup();
}
void loop(){
    switch(box){
        case 0://Hovedcase. Her kjører bilen med linjefølger og stopper når den når ønskede mål. Ellers kjører den over kryss i case 1.
            driveLinePID();
            if(aboveLine(0) && aboveLine(1) && aboveLine(2) && aboveLine(3) && aboveLine(4) && direction == 1 && (gps+1) == whereTo){
                box = 2;
                motors.setSpeeds(100,100);
                prevmillis = millis();
            }
            else if(aboveLine(0) && aboveLine(1) && aboveLine(2) && aboveLine(3) && aboveLine(4) && direction == 1){
                gps = gps+1;
                box = 1;
                prevmillis = millis();
            }
        break;

        case 1://Bilen kjører rett fram over krysset.
            motors.setSpeeds(100,100);
            if(millis()-prevmillis>200){
                box = 0;
            }
        break;

        case 2://Bilen stopper etter den har kjørt litt fremmover.
            if(millis()-prevmillis>250){
                motors.setSpeeds(0,0);
                turndeg(87);
                motors.setSpeeds(100,100);
                prevmillis = millis();
                if(whereTo == 3){//Utifra valgt plass(whereTo) så bytter den til neste case.
                    box = 3;
                }
                else if(whereTo == 2){
                    box = 4;
                }
            }
        break;

        case 3://Garasje case.
            
        break;
    }
  }