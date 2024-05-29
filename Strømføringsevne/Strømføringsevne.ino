#include <Wire.h>
#include <WiFi.h>
#include "SparkfunBME280.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

BME280 mySensor;
WiFiClient espClient;
PubSubClient client(espClient);

//Temperatur-variabler

int temperature = 0;

long lastMsg = 0;

// MQTT-variabler

const char* ssid = "NTNU-IOT";
const char* password = "";
const char* MQTT_UN = "fosfix";
const char* MQTT_PW = "C4nn3ds0up3s321";
const char* mqtt_server = "10.25.18.161";


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

void sendTempInfo(){
    long now = millis();
    if(now - lastMsg > 5000){
        lastMsg = now;

        //Lager et JSON-dokument som skal inneholde data
        StaticJsonDocument<80> doc;
        char output[80];

        //Legger til variabler til JSON-dokumentet
        doc["t1"] = temperature;

        serializeJson(doc, output); //Gjør om verdiene til noe som kan sendes
        Serial.println(output);
        client.publish("stromforing/maling1", output); //Sender dokumentet til MQTT
    }
}



void setup(){
    Serial.begin(9600);

    Wire.begin();

    if (mySensor.beginI2C() == false){
        Serial.println("The sensor did not respond. Please check wiring.");
    while(1); //Freeze
    }    

}

void loop(){
    temperature = mySensor.readTempF();
    
    sendTempInfo();
}