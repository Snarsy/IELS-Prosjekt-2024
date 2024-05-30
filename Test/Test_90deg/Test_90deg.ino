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

void setup(){
    lineSensors.initFiveSensors();
    turnSensorSetup();
    IR.begin(IRPin, ENABLE_LED_FEEDBACK);
}
void loop(){
    driveLinePID();
    if(aboveAll()){
        turndeg(90);
    }
}