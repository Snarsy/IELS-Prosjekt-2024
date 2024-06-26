#include <Wire.h>
#include <IRremote.h>

IRsend ir;

//Sendepins til distansemåler

const int trigPin_1 = 2;
const int trigPin_2 = 4;
const int trigPin_3 = 6;
const int trigPin_4 = 8;

int trigPinArray[4] = {trigPin_1, trigPin_2, trigPin_3, trigPin_4};


//Mottakerpins til distansemåler

const int echoPin_1 = 3;
const int echoPin_2 = 5;
const int echoPin_3 = 7;
const int echoPin_4 = 9;

int echoPinArray[4] = {echoPin_1, echoPin_2, echoPin_3, echoPin_4};


//Distansemåling variabler

int distance_spot1, distance_spot2, distance_spot3, distance_spot4;
int prevDistance_spot1, prevDistance_spot2, prevDistance_spot3, prevDistance_spot4; 

int parkingDistanceArray[4] = {distance_spot1, distance_spot2, distance_spot3, distance_spot4};
int previousParkingDistanceArray[4] = {prevDistance_spot1, prevDistance_spot2, prevDistance_spot3, prevDistance_spot4};

int parkingDistanceTreshold = 8; //Hvor lav målingen skal være for at noe skal være parkert
const float speedOfSound = 0.0343;

//Parkeringsstatusvariabler
bool parkingspace1_status, parkingspace2_status, parkingspace3_status, parkingspace4_status = false;

bool parkingStatusArray[4] = {parkingspace1_status, parkingspace2_status, parkingspace3_status, parkingspace4_status};


//Sensorer som sjekker om det er biler på parkeringsplassen eller ikke

void physicalCheck_parking(){
    long duration;

    for(int i = 0; i < 4; i++){

        digitalWrite(trigPinArray[i], LOW);
        delayMicroseconds(2);
        digitalWrite(trigPinArray[i], HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPinArray[i], LOW);
        duration = pulseIn(echoPinArray[i], HIGH);
        parkingDistanceArray[i] = (duration*speedOfSound)/2.0; //Måler tiden det tar og deler på to fordi lyden går frem og tilbake


        if (parkingDistanceArray[i] != previousParkingDistanceArray[i]){ //Dersom distansen fra taket endrer seg

            if(parkingDistanceArray[i] > parkingDistanceTreshold){ //Dersom det virker som det ikke er en bil på parkeringen
                parkingStatusArray[i] = false;
            }

            if(parkingDistanceArray[i] <= parkingDistanceTreshold){ //Dersom det virker som det er en bil på parkeringen
                parkingStatusArray[i] = true;
            }
        
            updateParkingStatus(i);
        }

            Serial.print("Sensor ");
            Serial.print(i+1);
            Serial.print(": ");
            Serial.println(parkingStatusArray[i]);
        previousParkingDistanceArray[i] = parkingDistanceArray[i];
    }
}

//Sender informasjon til ESP32 via I2C

void updateParkingStatus(int Parkingspace){
    Wire.beginTransmission(2);
    Wire.write(Parkingspace);
    Wire.write(parkingStatusArray[Parkingspace]);
    Wire.endTransmission();
}


void setup(){
    Serial.begin(9600);

    //Pins
    pinMode(trigPin_1 , OUTPUT);
    pinMode(trigPin_2 , OUTPUT);
    pinMode(trigPin_3 , OUTPUT);
    pinMode(trigPin_4 , OUTPUT);
    pinMode(echoPin_1, INPUT);
    pinMode(echoPin_2, INPUT);
    pinMode(echoPin_3, INPUT);
    pinMode(echoPin_4, INPUT);

    //Kommunikasjon med ESP32
    Wire.begin();
}

void loop(){
    physicalCheck_parking();
}
