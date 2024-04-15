#include <Zumo32U4.h>
Zumo32U4OLED display;
Zumo32U4Buzzer buzzer;
Zumo32U4ButtonA buttonA;
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
Zumo32U4IMU imu;

#include "TurnSensor.h"

int gps = 0;
int plass;

const int followLinemaxSpeed = 200;
int lastError = 0;
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
  switch (gps){
    case 0:
      driveLinePID();
      readSensors();
      if(aboveLine(0) && aboveLine(1) && aboveLine(2) && aboveLine(3) && aboveLine(4)){
        motors.setSpeeds(100,100);
        delay(300);
        motors.setSpeeds(0,0);
        delay(500);
        gps = 1;
      }
      break;

    case 1:
      motors.setSpeeds(0,0);
      if(buttonA.isPressed()){
        delay(500);
        gps = 2;
        plass = 2;
      }
      break;

    case 2:
      motors.setSpeeds(100,100);
      readSensors();
      if(aboveLine(3) && aboveLine(4) && aboveLine(0)!=1 && aboveLine(1)!=1){
        motors.setSpeeds(100,100);
        delay(300);
        motors.setSpeeds(0,0);
        if(plass == 0){
          gps = 0;
          turndeg(87);
        }
        else{
          plass--;
        }
      }
      break;
  }
}
int pop;