#include "Adafruit_APDS9960.h"
#include <WiFi.h>
#include <PubSubClient.h>


WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_APDS9960 apds;


// MQTT-variabler

const char* ssid = "NTNU-IOT";
const char* password = "";
const char* MQTT_UN = "fosfix";
const char* MQTT_PW = "C4nn3ds0up3s321";
const char* mqtt_server = "10.25.18.134";

//Manni $$$$$$

int transactionCaseNumber = 0;

//batterihelse
int battery

int maxCharge = 50;

// the setup function runs once when you press reset or power the board
void setup() {
  
    Serial.begin(9600);

    if(!apds.begin()){
      Serial.println("failed to initialize device! Please check your wiring.");
    }
    else Serial.println("Device initialized!");

    //gesture mode will be entered once proximity mode senses something close
    apds.enableProximity(true);
    apds.enableGesture(true);

    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

void receiveBatteryHealth(){
//Finne ut hvordan vi skal gjøre dette
}

void batterygestures(){
  //read a gesture from the device
  uint8_t gesture = apds.readGesture();

  if(gesture == APDS9960_DOWN){

  }

  if(gesture == APDS9960_UP){
    if(maxCharge == batteryCharge){

    }
      Serial.println("Price going up");
      boughtbatteryhealth += 10;
      newbatteryhealth = receivedBatteryHealth + boughtbatteryhealth;
      Serial.print("New battery health =");
      Serial.println(newbatteryhealth);        
  } 

  if(gesture == APDS9960_LEFT){
    transactionCaseNumber = 1;
  } 

  if(gesture == APDS9960_RIGHT){
    transactionCaseNumber = 2;
  } 
}

void doyouwanttocancel(){

  Serial.println("Do you want to cancel the transaction?");
  Serial.println("Left - No     Right - Yes");

  uint8_t gesture = apds.readGesture();

  if (gesture == APDS9960_LEFT){
    Serial.println("Back to menu");
    transactionCaseNumber = 0;
  }

  if (gesture == APDS9960_RIGHT){
    Serial.println("Goodbye");
  }
}

void doyouwanttobuy(){

  Serial.print("Do you want to buy ");
  Serial.print(boughtbatteryhealth);
  Serial.println("% ?");
  Serial.println("Left - No     Right - Yes");

  uint8_t gesture = apds.readGesture();

  if (gesture == APDS9960_LEFT){
    Serial.println("Back to menu");
    transactionCaseNumber = 0;
  }

  if (gesture == APDS9960_RIGHT){
    Serial.print("Bought ");
    Serial.print(boughtbatteryhealth);
    Serial.println("%");
    Serial.print("Batteryhealth: ");
    Serial.print(newbatteryhealth);
    Serial.println("%");
  }
}


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

void callback(String topic, byte *message, unsigned int length)
{
  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    //Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (topic == "stromforing/kfaktor")
  {
    float tempFactor = messageTemp.toFloat();
    
    if(tempFactor >= 1.11){
      maxCharge = 80;

    }else if(tempFactor >= 1.04 && tempFactor < 1.11){
      maxCharge = 60;

    }else if(tempFactor <= 1){
      maxCharge = 50;
    }
  }
}


// the loop function runs over and over again forever
void loop() {

  if (!client.connected()) {
        reconnect();
  }
  client.loop();  
  
  transactionCase();
}

void transactionCase(){
  switch(transactionCaseNumber){
    case 0:
    batterygestures();

    break;

    case 1:
    //Cancel transaction?
    doyouwanttocancel();
    break;

    case 2:
    // Purchase transaction?
    doyouwanttobuy();
    break;

    case 3:
    Serial.println("Goodbye");
    delay(1000);
    break;
  }
}