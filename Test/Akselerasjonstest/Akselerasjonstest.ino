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

unsigned long previousSpeedMillis = 0;
int speedDistance, totalDistance = 0;
int A = 1;
int16_t firstSpeed, totalSpeed = 0;
int readTime = 100;

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
        totalSpeed = abs((lastSpeed - firstSpeed) / 909.70 * 10.996 * 4); // Verdiene er regnet med hvor mange ganger den teller og areal av hjulet.
        speedDistance += totalSpeed / 10; // Deler på 10 siden den teller hvert 1/10 sekund.
    }
}

void setup(){
    motors.setSpeeds(100,100);
}
int speed = 100;
unsigned long prevmillis = 0;
int lastspeed = 0;
void loop(){
    if(millis()-prevmillis>5000){
        prevmillis = millis();
        speed = speed + 100;
    }
    speedometer();
    motors.setSpeeds(speed,speed);
    if(lastspeed != speedDistance){
        display.clear();
        lastspeed = speedDistance;
    }
    display.println(speedDistance);
}