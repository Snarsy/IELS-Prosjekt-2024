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

#define PRINT_DEGUB_INFO 0 // Set to 1 to enable debug info

#define SUBCARIER_PERIOD 420 // The default frequency is 16000000 / (420 + 1) = 38.005 kHz
#define IR_BRIGHTNESS 50     // 0-400, 0 is off, 400 is max brightness

#define RIGHT_IR 0
#define LEFT_IR 1

const int IRPin = A4;
// const int IRSenderPin = 5;
IRrecv IR(IRPin);
// IRsend irSender;

int irGarageCase = 0;

int gps = 0;

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

void setup()
{
    Serial.begin(9600);
    irSensors.stop();
    lineSensors.initFiveSensors();
    proxSensors.initFrontSensor();
    IR.begin(IRPin, ENABLE_LED_FEEDBACK);
    // irSender.begin(IRSenderPin);
    turnSensorSetup();
    turnSensorReset();
}

void irCase(){
    switch(irGarageCase){
    case 0:
        irDecode();
        break;
    case 1:

        break;
    case 2:

        break;
    case 3:

        break;
    case 4:

        break;
    case 5:

        break;
    case 6:

        break;

    }
}

void irDecode(){
    if (IR.decode())
    {
        Serial.println(IR.decodedIRData.decodedRawData);
        if (IR.decodedIRData.decodedRawData == 2592268650) irGarageCase = 1;
        if (IR.decodedIRData.decodedRawData == 556677) irGarageCase = 2;
        if (IR.decodedIRData.decodedRawData == 334455 ) irGarageCase = 3;
        if (IR.decodedIRData.decodedRawData == 112233 ) irGarageCase = 4;
        if (IR.decodedIRData.decodedRawData == 778899 ) irGarageCase = 5;
        IR.resume();
    }
}

void garageSwitch(){
    switch (gps){
    case 0:
      driveLinePID();
      irDecode();
      readSensors();
      break;

    case 1:
      driveLinePID();
      readSensors();
      if(aboveLine(0) && aboveLine(1) && aboveLine(2) && aboveLine(3) && aboveLine(4)){
        motors.setSpeeds(100,100);
        delay(300);
        motors.setSpeeds(0,0);
        gps = 2;
        delay(500);
      }
      break;

    case 2:
      motors.setSpeeds(100,100);
      readSensors();
      if(aboveLine(3) && aboveLine(4) && aboveLine(0)!=1 && aboveLine(1)!=1){
        motors.setSpeeds(100,100);
        delay(300);
        motors.setSpeeds(0,0);
        turndeg(87);
        gps = 0;
      }
      break;
  }
}


void loop()
{
   garageSwitch();
}