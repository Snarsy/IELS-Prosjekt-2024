#include <Zumo32U4.h>
Zumo32U4OLED display;
Zumo32U4Buzzer buzzer;
Zumo32U4ButtonA buttonA;
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
Zumo32U4IMU imu;

#include "InterScript.h"
#include "TurnSensor.h"


unsigned long prevmillis = 0;
int vinkel;
bool sjekk1=0;
bool sjekk2=0;
bool drive = 1;
bool inGarage = 0;
bool onRoad = 0;

void turndeg(int tilverdi)//Denne funksjonen roterer bilen i 90 grader ved hjelp av gyroen. Den har 87 grader ettersom dette er hva 90 grader er lik på skolens gulv.
//Derimot med mer friksjon så vil man bruke 90 grader istedenfor 87 for 90.
//Denne er blokkerende, men det er grunnet at dette er en rotasjon og tilnærmet null batteriliv skal gå ned av dette.
{ // 87 grader er lik 90
  // Snur den 90 grader. 
    delay(5);
    sjekk1 = 0;
    sjekk2 = 0;
    tilverdi += 180;
    turnSensorReset();
    while (vinkel <= tilverdi || vinkel >= tilverdi) 
    {
        turnSensorUpdate();
        vinkel = ((((int32_t)turnAngle >> 16) * -360) >> 16) + 180;
        if (vinkel < tilverdi)
        {
            sjekk1 = 1;
            motors.setSpeeds(100, -100);
        }
        else if (vinkel > tilverdi)
        {
            sjekk2 = 1;
            motors.setSpeeds(-100, 100);
        }
        if (sjekk1 && sjekk2)
        {
            motors.setSpeeds(0, 0);
            break;
        }
    }
}
void turnreset(){
    turnSensorReset();
    sjekk1=0;
    sjekk2=0;
    drive=1;
}

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
  turnSensorReset();
}
void loop(){
  if(buttonA.isPressed()){
    delay(500);
    onRoad = 1;
  }
  if(onRoad == 1){
    driveLinePID();
  }
  readSensors();
  if (inGarage == 0){
    if(aboveLine(0) && aboveLine(1) && aboveLine(2) && aboveLine(3) && aboveLine(4)){
    delay(400);
    motors.setSpeeds(0,0);
    onRoad = 0;
    inGarage = 1;
    delay(300);
    }
  }
  else if (inGarage == 1){
    if(aboveLine(2) && aboveLine(3) && aboveLine(4) && onRoad == 1){
      delay(400);
      motors.setSpeeds(0,0);
      inGarage = 0;
      onRoad = 0;
      turndeg(87);
    }
  }
}