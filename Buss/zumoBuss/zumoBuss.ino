#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4OLED display;
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
Zumo32U4Buzzer buzzer;
Zumo32U4IMU imu;


byte RxByte;

int speedCaseInt = 0;

unsigned int lineSensorValues1[5] = {0, 0, 0, 0, 0};

#include "TurnSensor.h"



int Lspeed = 0;
int Rspeed = 0;

void I2C_RxHandler(int numBytes)
{
  while(Wire.available()) {  // Read Any Received Data
    RxByte = Wire.read();
  }
}
 
void setup() {
  lineSensors.initFiveSensors();
  Wire.begin(0x55); // Initialize I2C (Slave Mode: address=0x55 )
  Wire.onReceive(I2C_RxHandler);
  Serial.begin(9600);
  display.setLayout21x8();
  display.clear();
  //turnSensorSetup();
  delay(1000);
  calibrating();
}
 
void calibrating()
{ //Kalibrerer linjesensor
    for (int i = 0; i < 60; i++)
    {
        motors.setSpeeds(200, -200);
        lineSensors.calibrate();
        delay(10);
    }
    display.clear();
    motors.setSpeeds(0, 0);
    delay(1000);
}



    motors.setSpeeds(Lspeed, Rspeed);
}

void receiveByte(){
    if (RxByte == 2){ //Slow speed
        speedCaseInt = 1;
    }
    else if (RxByte == 1) //High speed
    {
        speedCaseInt = 0;
    }
    
}

void fullstop(){
    motors.setSpeeds(0,0);
}

void busstop(){
    if(aboveAll()){
        motors.setSpeeds(100,100);
        delay(100);
        motors.setSpeeds(0,0);
        // Play G4 (392 Hz)
        buzzer.playFrequency(392, 100, 15); // Frequency, duration in ms, volume (0-15)
        delay(100); // 250ms tone + 100ms delay

        // Play D5 (587 Hz)
        buzzer.playFrequency(587, 100, 15); // Frequency, duration in ms, volume (0-15)
        delay(100); // 250ms tone + 100ms delay

        // Play G5 (784 Hz)
        buzzer.playFrequency(784, 100, 15); // Frequency, duration in ms, volume (0-15)
        delay(100); // 250ms tone + 100ms delay
        delay(5000);
    }
}

void speedCase(){
    switch (speedCaseInt)
    {
    case 0:
            driveLineStandardHighSpeed();
        break;
    
    case 1:
            driveLineStandardLowSpeed();
        break;
    
    case 2:
            fullstop();
    }
}



void loop() {
  speedCase();
  receiveByte();
  busstop();
}