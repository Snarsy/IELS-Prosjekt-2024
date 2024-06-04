#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4OLED display;
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
Zumo32U4Buzzer buzzer;
Zumo32U4IMU imu;


byte RxByte;

int speedCaseInt = 0;


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
  turnSensorSetup();
  delay(1000);
}

void driveLinePIDNormalSpeed()
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

void driveLinePIDSlowSpeed(){
    int16_t position = lineSensors.readLine(lineSensorValues); //Leser linjeesensor. 
    int16_t error = position - 2000; //Finnet ut om det er venstre eller høyre som er mer utenfor linjen.
    int16_t speedDifference = error / 1 + 4 * (error - lastError); 

    lastError = error;

    int leftSpeed = followLinemaxSpeed*0.5 + speedDifference;
    int rightSpeed = followLinemaxSpeed*0.5 - speedDifference;
    leftSpeed = constrain(leftSpeed, 0, (int16_t)followLinemaxSpeed); //Constrain for å ikke få minus, og ikke over 400 verdi. 
    rightSpeed = constrain(rightSpeed, 0, (int16_t)followLinemaxSpeed);

    motors.setSpeeds(leftSpeed, rightSpeed);
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
        buzzer.playFrequency(392, 250, 15); // Frequency, duration in ms, volume (0-15)
        delay(250); 

        // Play D5 (587 Hz)
        buzzer.playFrequency(587, 250, 15); // Frequency, duration in ms, volume (0-15)
        delay(250); 

        // Play G5 (784 Hz)
        buzzer.playFrequency(784, 250, 15); // Frequency, duration in ms, volume (0-15)
        delay(250); 
        delay(5000);
    }
}

void speedCase(){
    switch (speedCaseInt)
    {
    case 0:
            driveLinePIDNormalSpeed();
        break;
    
    case 1:
            driveLinePIDSlowSpeed();
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