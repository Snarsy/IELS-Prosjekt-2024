void calibrateSensors()
{
  display.clear();
  delay(1000);
  motors.setSpeeds(200,-200);
  while(millis()<2600){
    lineSensors.calibrate();
  }
  motors.setSpeeds(0, 0);
}

void turndeg(int tilverdi){
    unsigned long turnmillis = millis();
    while(millis()-turnmillis<1400){
        if(tilverdi == 90){
            motors.setSpeeds(100,-100);
        }
        else if(tilverdi == -90){
            motors.setSpeeds(-100,100);
        }
        else if(tilverdi == 180){
            motors.setSpeeds(150,-150);
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
    return lineSensorValues[sensorIndex] > sensorThreshold;
}


int followLinemaxSpeed = 350;
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
    if(aboveLine(3) && aboveLine(4)){
        return true;
    }
    else{
        return false;
    }
}
bool aboveLeft(){
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

void batterycheck() // Måler fart hvert 10.dels sekund. Siden readtime = 100.
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
    batterylevel = 100 - speedDistance/5;

    if(batterylevel < 30){
        destination = 6; //Setter denne til ladestasjon slik at bilen vil kjøre dit for å lade.
    }
}