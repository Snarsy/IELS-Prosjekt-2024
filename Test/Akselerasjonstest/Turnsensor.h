// Turnsensor.h provides functions for configuring the
// Zumo 32U4's gyro, calibrating it, and using it to
// measure how much the robot has turned about its Z axis.
//
// This file should be included once in your sketch,
// somewhere after you define objects named buttonA,
// display, and imu.

#pragma once

#include <Wire.h>

// This constant represents a turn of 45 degrees.
const int32_t turnAngle45 = 0x20000000;

// This constant represents a turn of 90 degrees.
const int32_t turnAngle90 = turnAngle45 * 2;

// This constant represents a turn of approximately 1 degree.
const int32_t turnAngle1 = (turnAngle45 + 22) / 45;

/* turnAngle is a 32-bit unsigned integer representing the amount
the robot has turned since the last time turnSensorReset was
called.  This is computed solely using the Z axis of the gyro, so
it could be inaccurate if the robot is rotated about the X or Y
axes.

Our convention is that a value of 0x20000000 represents a 45
degree counter-clockwise rotation.  This means that a uint32_t
can represent any angle between 0 degrees and 360 degrees.  If
you cast it to a signed 32-bit integer by writing
(int32_t)turnAngle, that integer can represent any angle between
-180 degrees and 180 degrees. */
uint32_t turnAngle = 0;

// turnRate is the current angular rate of the gyro, in units of
// 0.07 degrees per second.
int16_t turnRate;

// This is the average reading obtained from the gyro's Z axis
// during calibration.
int16_t gyroOffset;

// This variable helps us keep track of how much time has passed
// between readings of the gyro.
uint16_t gyroLastUpdate = 0;


// This should be called to set the starting point for measuring
// a turn.  After calling this, turnAngle will be 0.
void turnSensorReset()
{
  gyroLastUpdate = micros();
  turnAngle = 0;
}

// Read the gyro and update the angle.  This should be called as
// frequently as possible while using the gyro to do turns.
void turnSensorUpdate()
{
  // Read the measurements from the gyro.
  imu.readGyro();
  turnRate = imu.g.z - gyroOffset;

  // Figure out how much time has passed since the last update (dt)
  uint16_t m = micros();
  uint16_t dt = m - gyroLastUpdate;
  gyroLastUpdate = m;

  // Multiply dt by turnRate in order to get an estimation of how
  // much the robot has turned since the last update.
  // (angular change = angular velocity * time)
  int32_t d = (int32_t)turnRate * dt;

  // The units of d are gyro digits times microseconds.  We need
  // to convert those to the units of turnAngle, where 2^29 units
  // represents 45 degrees.  The conversion from gyro digits to
  // degrees per second (dps) is determined by the sensitivity of
  // the gyro: 0.07 degrees per second per digit.
  //
  // (0.07 dps/digit) * (1/1000000 s/us) * (2^29/45 unit/degree)
  // = 14680064/17578125 unit/(digit*us)
  turnAngle += (int64_t)d * 14680064 / 17578125;
}

/* This should be called in setup() to enable and calibrate the
gyro.  It uses the display, yellow LED, and button A.  While the
display shows "Gyro cal", you should be careful to hold the robot
still.

The digital zero-rate level of the gyro can be as high as
25 degrees per second, and this calibration helps us correct for
that. */
void turnSensorSetup()
{
  Wire.begin();
  imu.init();
  imu.enableDefault();
  imu.configureForTurnSensing();

  display.clear();
  display.print(F("Gyro cal"));

  // Calibrate the gyro.
  int32_t total = 0;
  for (uint16_t i = 0; i < 1024; i++)
  {
    // Wait for new data to be available, then read it.
    while(!imu.gyroDataReady()) {}
    imu.readGyro();

    // Add the Z axis reading to the total.
    total += imu.g.z;
  }
  ledYellow(0);
  gyroOffset = total / 1024;

  // Display the angle (in degrees from -180 to 180) until the
  // user presses A.
  display.clear();
  turnSensorReset();
  while (millis()<5300)
  {
    motors.setSpeeds(100,-100);
    turnSensorUpdate();
    lineSensors.calibrate();
    display.gotoXY(0, 0);
    display.print(((((int32_t)turnAngle >> 16) * -360) >> 16)+180);
    display.print(F("   "));
  }
  display.clear();
  motors.setSpeeds(0,0);
}
int vinkel;
bool sjekk1=0;
bool sjekk2=0;
bool drive = 1;
void turndeg(int tilverdi){//Denne funksjonen roterer bilen i 90 grader ved hjelp av gyroen. Den har 87 grader ettersom dette er hva 90 grader er lik på skolens gulv. Derimot med mer friksjon så vil man bruke 90 grader istedenfor 87 for 90. Denne er blokkerende, men det er grunnet at dette er en rotasjon og tilnærmet null batteriliv skal gå ned av dette.
    // 87 grader er lik 90
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
const uint8_t numSensors = 5;
const uint16_t sensorThreshold = 200;
uint16_t lineSensorValues[numSensors];
uint16_t readSensors()
{
  return lineSensors.readLine(lineSensorValues);
}

bool aboveLine(uint8_t sensorIndex)
{
    readSensors();
    return lineSensorValues[sensorIndex] > sensorThreshold;
}


int followLinemaxSpeed = 200;
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

bool aboveRight(){
    readSensors();
    if(aboveLine(3) && aboveLine(4)){
        return true;
    }
    else{
        return false;
    }
}
bool aboveLeft(){
    readSensors();
    if(aboveLine(0) && aboveLine(2)){
        return true;
    }
    else{
        return false;
    }
}
bool aboveAll(){
    if(aboveLine(0) && aboveLine(1) && aboveLine(2) && aboveLine(3) && aboveLine(4)){
        return true;
    }
    else{
        return false;
    }
}