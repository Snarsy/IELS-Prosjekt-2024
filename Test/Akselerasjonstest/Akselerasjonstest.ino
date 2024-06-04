#include <Zumo32U4.h>
#include <Wire.h>
#include <EEPROM.h>

Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4OLED display;
Zumo32U4Encoders encoder;
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensor;
Zumo32U4Buzzer buzzer;
Zumo32U4IMU imu;
#include "Turnsensor.h"

unsigned long previousSpeedMillis, prevAveragesMillis = 0;
int speedDistance, totalDistance = 0;
int A = 1;
int16_t firstSpeed = 0;
int readTime = 100;
int16_t totalSpeed = 0;
int16_t negativeTotalSpeed = 0;
bool seventyMillis_start, stoppedTimer = false;
int holdTimerValue, secondsAboveSeventy, aboveSeventyCounter, maxSpeed, distanceAverage, averageSpeed60Sec = 0;
int akselerasjon;
int prevSpeed;

void speedometer() // Måler fart hvert 10.dels sekund. Siden readtime = 100.
{
    unsigned long speedMillis = millis();
    if (A == 1)
    {
        firstSpeed = encoder.getCountsLeft() + encoder.getCountsRight();
        A = 0;
    }
    if (speedMillis - previousSpeedMillis > readTime)
    {
        int16_t lastSpeed = encoder.getCountsLeft() + encoder.getCountsRight();
        A = 1;
        previousSpeedMillis = speedMillis;
        totalSpeed = abs((lastSpeed - firstSpeed) / 909.70 * 10.996 * 4); // Verdiene er regnet med hvor mange ganger den teller og areal av hjulet. Får det ut i cm/s
        negativeTotalSpeed = totalSpeed * -1;
        speedDistance += totalSpeed / 10; // Deler på 10 siden den teller hvert 1/10 sekund.
        totalDistance += speedDistance;
        distanceAverage += totalSpeed / 10;
        akselerasjon = (totalSpeed-prevSpeed);
        prevSpeed = totalSpeed;
    }
}


void setup(){
    lineSensors.initFiveSensors();
    //turnSensorSetup();
}
void loop(){
    speedometer();
    if(millis()%50 == 0){
        display.clear();
        display.println(akselerasjon);
    }
    if(millis()<10000 ){
        motors.setSpeeds(100,100);
    }
    if(millis()>15000){
        motors.setSpeeds(400,400);
    }
    if(millis()>30000){
        motors.setSpeeds(0,0);
    }
}