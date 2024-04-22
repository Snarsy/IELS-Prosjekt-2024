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

IRrecv IR(IRPin);


int irGarageCase = 0;



void setup()
{
    Serial.begin(9600);
    lineSensors.initFiveSensors();
    proxSensors.initFrontSensor();
    IR.begin(IRPin, ENABLE_LED_FEEDBACK);
    
}

void irCase(){
    switch(irGarageCase){
    case 0:
        irDecode();
        break;
    case 1:
        //En
        break;
    case 2:
        //To
        break;
    case 3:
        //Tre
        break;
    case 4:
        //Fire
        break;
    case 5:

        break;
    case 6:
        //Ingen ledig plass
        break;

    }
}

void irDecode(){
    if (IR.decode())
    {
        Serial.println(IR.decodedIRData.decodedRawData);
        if (IR.decodedIRData.decodedRawData == 2592268650) irGarageCase = 1; //Første verdi
        if (IR.decodedIRData.decodedRawData == 510274632) irGarageCase = 2; //Andre verdi
        if (IR.decodedIRData.decodedRawData == 1277849113 ) irGarageCase = 3; //Tredje verdi
        if (IR.decodedIRData.decodedRawData == 2163717077 ) irGarageCase = 4; //Fjerde verdi
        if (IR.decodedIRData.decodedRawData == 2227349217 ) irGarageCase = 5; //Femte verdi
        IR.resume();
    }
}




void loop()
{
   irCase();
}