#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <esp_sleep.h>
#include <IRrecv.h>
#include <Servo.h>


// MQTT-variabler

const char* ssid = "NTNU-IOT";
const char* password = "";
const char* MQTT_UN = "fosfix";
const char* MQTT_PW = "C4nn3ds0up3s321";
const char* mqtt_server = "10.25.18.134";


// Parkeringvariabler

volatile int parkingSpace, availability = -1;

const int spaceNumber = 4;

int numberOfSpotsAdress = 1;
int previousNumberOfSpots = 0;
int availability_spot1 , availability_spot2, availability_spot3, availability_spot4 = 0;
int availabilityArray[spaceNumber] = {availability_spot1, availability_spot2, availability_spot3, availability_spot4};

long lastMsg = 0;
long parkingTimer;
long lastParkingTimer;

bool noSpotsAvailable = false;


// LED-pins

const int ledPin_1 = 12;
const int ledPin_2 = 14;
const int ledPin_3 = 26;
const int ledPin_4 = 27;

int ledPinArray[spaceNumber] = {ledPin_1, ledPin_2, ledPin_3, ledPin_4};


// IR-variabler

const uint16_t IRPin = 32;  // ESP32 pin GPIO 32
const uint16_t IRRecievePin = 33; //ESP32 pin GPIO 33

const int sleep_delay = 10000;
const int hexForIR_parkingSpace1 = 0x56874159;
const int hexForIR_parkingSpace2 = 0x12345678;
const int hexForIR_parkingSpace3 = 0x98765432;
const int hexForIR_parkingSpace4 = 0xABCDEF01;
const int hexForIR_noParking = 0x87654321;
const int hexForIR_ElectricCar = 3292233855;

int hexForIR_Array[spaceNumber] = {hexForIR_parkingSpace1 ,hexForIR_parkingSpace2, hexForIR_parkingSpace3 ,hexForIR_parkingSpace4};

int randomLedigPlass;
int sisteSendtePlass;


//Servo-variabler

static const int servoPin = 4;


//Biblioteksinitiering
IRsend ir(IRPin);
WiFiClient espClient;
PubSubClient client(espClient);
IRrecv irrecv(IRRecievePin);
decode_results results;
Servo servo1;


// MQTT & WiFi setup

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");

        //Lager clientID
        String clientId = "ESP32Client-";
        clientId += String(random(0xffff), HEX);

        //Prøver å koble seg på serveren
        if(client.connect(clientId.c_str(), MQTT_UN, MQTT_PW)){
            Serial.println("Connected!");
        } 
        else{
            Serial.print("Failed, code=");
            Serial.println(client.state());
            delay(5000);
        }
    }   
}

void clientell(){

    //Kobler ESP på WiFi
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
}


void sendParkInfo(){
    long now = millis();
    if(now - lastMsg > 5000){
        lastMsg = now;

        //Lager et JSON-dokument som skal inneholde data
        StaticJsonDocument<80> doc;
        char output[80];

        //Legger til variabler til JSON-dokumentet
        doc["n"] = previousNumberOfSpots;

        serializeJson(doc, output); //Gjør om verdiene til noe som kan sendes
        Serial.println(output);
        client.publish("garage/esp32", output); //Sender dokumentet til MQTT
    }
}


// I2C-kommunikasjon med Arduino


void receiveEvent(int howMany){
    while(1 < Wire.available())
    {
        parkingSpace = Wire.read();
    }
    if(Wire.available())
    {
        availability = Wire.read();
    }
}


void updateNumberOfSpots(){
    //Bruker parkeringslysene for å telle antall ledige plasser
    int numberOfSpots = 0;
    for(int i = 0; i < spaceNumber; i++){

        if(digitalRead(ledPinArray[i]) == LOW){
            availabilityArray[i] = 0;
        }

        else if(digitalRead(ledPinArray[i]) == HIGH){
                lastParkingTimer = parkingTimer;
                numberOfSpots += 1;
                noSpotsAvailable = false;
            if(randomLedigPlass == i && parkingTimer - lastParkingTimer > 30000){ //Dersom det har tatt 30sek siden siste plass ble sendt ut og det ikke står noen der
                availabilityArray[i] = 1;
            }else{
                availabilityArray[i] = 1;
                }
        }
    }

    //Dersom antall ledige plasser endres
    if(numberOfSpots != previousNumberOfSpots){
        previousNumberOfSpots = numberOfSpots;
    }

    //Dersom det ikke er noen ledige plasser
    if(numberOfSpots == 0){
        noSpotsAvailable = true;
    }
}


// LED-Styring

void availabilityLEDs(){
    switch (parkingSpace)
    {
    case 0:
        digitalWrite(ledPin_1, !availability);
        break;

    case 1:
        digitalWrite(ledPin_2, !availability);
        break;

    case 2:
        digitalWrite(ledPin_3, !availability);
        break;

    case 3:
        digitalWrite(ledPin_4, !availability);
        break;
    
    default:
        break;
    }
}


//IR


void IR_for_parking(){

    //Fikk et IR-signal
    if(irrecv.decode(&results)){

        //Dersom ingen ledige plasser, si ifra til bil
        if(results.value == 3292217535){
            Serial.println("Access denied");
            ir.sendNEC(hexForIR_noParking, 32);
        }

        else if (results.value == 3292233855){

            //Dersom ingen ledige plasser, si ifra til bil
            if(noSpotsAvailable){
                delay(100);
                ir.sendNEC(hexForIR_noParking, 32);
            }

            //Ellers send ut en av plassene
            else{
                Serial.println("Åpner");
                for(int posDegrees = 0; posDegrees <= 90; posDegrees++) {
                    servo1.write(posDegrees);
                    delay(20);
                }
                
                //Sjekker alle parkeringsplasser og sjekker om de er ledige
                for(int i = 0; i < spaceNumber; i++){
                    Serial.println(randomLedigPlass);
                    if(availabilityArray[i] == 1 && sisteSendtePlass != i){ //Dersom plassen er ledig og ikke ble sendt ut sist, send ut signal
                        ir.sendNEC(hexForIR_Array[i], 32);
                        Serial.print("Sender: ");
                        Serial.println(i);
                        availabilityArray[i] = 0;
                        parkingTimer = millis();
                        randomLedigPlass = i;
                        i = spaceNumber; //avslutt for-løkken
                    }
                }

                //Ikke send fler enn ett signal, og ikke send det siste sendte. 
                sisteSendtePlass = randomLedigPlass; 
                delay(10000); //Venter til bilen har kjørt inn
                Serial.println("Lukker!");
                for(int posDegrees = 90; posDegrees > 0; posDegrees--) {
                    servo1.write(posDegrees);
                    delay(20);
                }
            }
        } 
        irrecv.resume();  // Få neste IR-verdi
    }
}

//MAIN


void setup(){
    //Seriell kommunikasjon
    Serial.begin(9600);

    //Pins
    pinMode(ledPin_1, OUTPUT);
    pinMode(ledPin_2, OUTPUT);
    pinMode(ledPin_3, OUTPUT);
    pinMode(ledPin_4, OUTPUT);

    //Servo
    servo1.attach(servoPin);

    //I2C-kommunikasjon
    Wire.begin(2);
    Wire.onReceive(receiveEvent);

    //Wifi-setup
    setup_wifi();
    client.setServer(mqtt_server, 1883);

    //IR-setup
    irrecv.enableIRIn();
    ir.begin();
}


void loop(){ 
    clientell();
    availabilityLEDs();
    updateNumberOfSpots();
    IR_for_parking();
    sendParkInfo();   
}
