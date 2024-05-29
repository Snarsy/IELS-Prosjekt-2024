#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4OLED display;
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;

byte RxByte;

int speedCaseInt = 0;

unsigned int lineSensorValues[5] = {0, 0, 0, 0, 0};

int Lspeed = 0;
int Rspeed = 0;

void I2C_RxHandler(int numBytes)
{
  while(Wire.available()) {  // Read Any Received Data
    RxByte = Wire.read();
  }
}
 
void setup() {
  Wire.begin(0x55); // Initialize I2C (Slave Mode: address=0x55 )
  Wire.onReceive(I2C_RxHandler);
  Serial.begin(9600);
  display.setLayout21x8();
  display.clear();
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
}

void driveLineStandardHighSpeed()
{ //Linjefølging der den ser hvilke side som er mer utenfor linje, og så gir meg motorkraft til den motoren som er utenfor.
    int Read = lineSensors.readLine(lineSensorValues);
    if (Read < 2000)
    {
        if (Read < 1750)
        {
            Lspeed = 100;
            Rspeed = 300;
        }
        else
        {
            Lspeed = 100;
            Rspeed = 200;
        }
    }
    else if (Read > 2000)
    {
        if (Read > 2250)
        {
            Lspeed = 300;
            Rspeed = 100;
        }
        else
        {
            Lspeed = 200;
            Rspeed = 100;
        }
    }

    motors.setSpeeds(Lspeed, Rspeed);
}

void driveLineStandardLowSpeed(){
     int Read = lineSensors.readLine(lineSensorValues);
    if (Read < 2000)
    {
        if (Read < 1750)
        {
            Lspeed = 50;
            Rspeed = 150;
        }
        else
        {
            Lspeed = 40;
            Rspeed = 60;
        }
    }
    else if (Read > 2000)
    {
        if (Read > 2250)
        {
            Lspeed = 150;
            Rspeed = 50;
        }
        else
        {
            Lspeed = 60;
            Rspeed = 40;
        }
    }

    motors.setSpeeds(Lspeed, Rspeed);
}

void receiveByte(){
    if (RxByte == 2){ //Slow speed
        speedCaseInt = 1;
        Serial.println("huh");
    }
    else if (RxByte == 1) //High speed
    {
        speedCaseInt = 0;
        Serial.println("high sped");
    }
    
}

void fullstop(){
    motors.setSpeeds(0,0);
}

void speedCase(){
    switch (speedCaseInt)
    {
    case 0:
            //driveLineStandardHighSpeed();
            motors.setSpeeds(200,200);
        break;
    
    case 1:
            //driveLineStandardLowSpeed();
            motors.setSpeeds(-200,-200);
        break;
    
    case 2:
            fullstop();
    }
}



void loop() {
  speedCase();
  receiveByte();
  Serial.println(RxByte);
}