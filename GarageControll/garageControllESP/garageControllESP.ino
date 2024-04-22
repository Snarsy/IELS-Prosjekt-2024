#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>


//MQTT-variabler

const char* ssid = "";
const char* password = "";
const char* MQTT_UN = "";
const char* MQTT_PW = "";
const char* mqtt_server = "";

//I2C-variabler

volatile int parkingSpace = -1;
volatile int availability = -1;

// LED-pins

const int ledPin_1 = 12;
const int ledPin_2 = 15;
const int ledPin_3 = 26;
const int ledPin_4 = 27;


WiFiClient espClient;
PubSubClient client(espClient);


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
    
    if (howMany >= 2){
        parkingSpace = Wire.read();
        availability = Wire.read();
    }
    
    /*
    int parkingSpace;
    while(1 < Wire.available())
    {
        int parkingSpace = Wire.read();
        Serial.print(parkingSpace);
    }
    int availability = Wire.read();
    Serial.println(availability);

    

    //Lager et JSON-dokument som skal inneholde data
    StaticJsonDocument<80> doc;
    char output[80];

    //Legger til variabler til JSON-dokumentet
    doc["s"] = parkingSpace;
    doc["a"] = availability;

    SerializeJson(doc, output); //Gjør om verdiene til noe som kan sendes
    Serial.println(output);
    client.publish("garage/esp32", output); //Sender dokumentet til MQTT
    */
}


// LED-Styring


void availabilityLEDs(){
    switch (parkingSpace)
    {
    case 0:
        digitalWrite(ledPin_1, availability);
        break;

    case 1:
        digitalWrite(ledPin_2, availability);
        break;

    case 2:
        digitalWrite(ledPin_3, availability);
        break;

    case 3:
        digitalWrite(ledPin_4, availability);
        break;
    
    default:
        break;
    }
}

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
  client.loop();

  if (parkingSpace != -1 && availability != -1) {

    //Endrer "tilgjengelighetslys"
    availabilityLEDs();

    //Lager et JSON-dokument som skal inneholde data
    StaticJsonDocument<80> doc;
    char output[80];

    //Legger til variabler til JSON-dokumentet
    doc["s"] = parkingSpace;
    doc["a"] = availability;

    serializeJson(doc, output); //Gjør om verdiene til noe som kan sendes
    Serial.println(output);
    client.publish("garage/esp32", output); //Sender dokumentet til MQTT

    //Resetter variabler
    parkingSpace = -1;
    availability = -1;
  }
}