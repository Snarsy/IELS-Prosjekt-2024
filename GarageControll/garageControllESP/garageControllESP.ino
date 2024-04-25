#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <EEPROM.h>


WiFiClient espClient;
PubSubClient client(espClient);


//MQTT-variabler

const char* ssid = "NTNU-IOT";
const char* password = "";
const char* MQTT_UN = "fosfix";
const char* MQTT_PW = "C4nn3ds0up3s321";
const char* mqtt_server = "10.25.18.161";


//Parkingvariabler

volatile int parkingSpace, availability = -1;

int numberOfSpotsAdress = 1;
int numberOfSpots = EEPROM.read(numberOfSpotsAdress);

long lastMsg = 0;


// LED-pins

const int ledPin_1 = 12;
const int ledPin_2 = 14;
const int ledPin_3 = 26;
const int ledPin_4 = 27;

int ledPinArray[4] = {ledPin_1, ledPin_2, ledPin_3, ledPin_4};


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
    Serial.print("parkingSpace: ");
    Serial.println(parkingSpace);
    Serial.print("Availability: ");
    Serial.println(availability);
}


void updateNumberOfSpots(){
    numberOfSpots = 0;
    for(int i = 0; i < 4; i++){
        if(digitalRead(ledPinArray[i]) == HIGH){
            numberOfSpots += 1;
        }
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


//MAIN


void setup(){
    Serial.begin(9600);

    pinMode(ledPin_1, OUTPUT);
    pinMode(ledPin_2, OUTPUT);
    pinMode(ledPin_3, OUTPUT);
    pinMode(ledPin_4, OUTPUT);

    Wire.begin(2);
    Wire.onReceive(receiveEvent);

    setup_wifi();
    client.setServer(mqtt_server, 1883);
}


void loop(){ 
    delay(100);

    if (!client.connected()) {
        reconnect();
    }
    updateNumberOfSpots();
    client.loop();
    availabilityLEDs();
    
    long now = millis();
        if(now - lastMsg > 5000){
        lastMsg = now;

        //Endrer "tilgjengelighetslys"

        //Lager et JSON-dokument som skal inneholde data
        StaticJsonDocument<80> doc;
        char output[80];

        //Legger til variabler til JSON-dokumentet
        doc["n"] = numberOfSpots;

        serializeJson(doc, output); //Gjør om verdiene til noe som kan sendes
        Serial.println(output);
        client.publish("garage/esp32", output); //Sender dokumentet til MQTT
        }
}
