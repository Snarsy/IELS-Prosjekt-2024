#include <Wire.h>
#include <Adafruit_APDS9960.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

const int IRPin = 32;

IRsend irsend(IRPin);
WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_APDS9960 apds;



// MQTT-variabler

const char* ssid = "NTNU-IOT";
const char* password = "";
const char* MQTT_UN = "fosfix";
const char* MQTT_PW = "C4nn3ds0up3s321";
const char* mqtt_server = "10.25.18.134";


//IR-variabler
const int hexForIR_howMuchCharge0 = 0x11111111;
const int hexForIR_howMuchCharge1 = 0x11111112;
const int hexForIR_howMuchCharge2 = 0x11111122;
const int hexForIR_howMuchCharge3 = 0x11111222;
const int hexForIR_howMuchCharge4 = 0x11112222;
const int hexForIR_howMuchCharge5 = 0x11122222;
const int hexForIR_howMuchCharge6 = 0x11222222;
const int hexForIR_howMuchCharge7 = 0x12222222;
const int hexForIR_howMuchCharge8 = 0x22222222;



//Batteri-variabler
int batteryCharge = 50;
int maxCharge = 50;
int transactionCaseNumber = 0;


void setup() {
    //Seriell komunikasjon
    Serial.begin(9600);

    //Bevegelsesføler
    if(!apds.begin()){
      Serial.println("failed to initialize device! Please check your wiring.");
    }
    else Serial.println("Device initialized!");

    apds.enableProximity(true);
    apds.enableGesture(true);

  
    //IR-setup
    irsend.begin();

    //Wifi
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

void batterygestures(){

  uint8_t gesture = apds.readGesture();

  if(gesture == APDS9960_DOWN){
    batteryCharge -= 10;
    Serial.println("1");

  }

  if(gesture == APDS9960_UP){
    if(maxCharge == batteryCharge){
      Serial.println("3");
      delay(3000);
    } else{
      batteryCharge += 10;
      Serial.println("2");
    } 
  } 

  if(gesture == APDS9960_LEFT){
    transactionCaseNumber = 1;
    Serial.print("11");

  } 

  if(gesture == APDS9960_RIGHT){
    transactionCaseNumber = 2;
    Serial.print("12");
  } 
}

void doyouwanttocancel(){

  uint8_t gesture = apds.readGesture();

  if (gesture == APDS9960_LEFT){
    transactionCaseNumber = 0;
    Serial.println("11");
  }

  if (gesture == APDS9960_RIGHT){
    batteryCharge = 0;
    transactionCaseNumber = 3;
    
    Wire.beginTransmission(4); // transmit to device #4
    Wire.write("12");
    Wire.endTransmission();
  }
}

void doyouwanttobuy(){

  uint8_t gesture = apds.readGesture();

  if (gesture == APDS9960_LEFT){
    transactionCaseNumber = 0;
    Serial.print("Left!");
    Wire.beginTransmission(4); // transmit to device #4
    Wire.write("11");
    Wire.endTransmission();
  }

  if (gesture == APDS9960_RIGHT){
    transactionCaseNumber = 3;
    Serial.print("RIght!");
    Wire.beginTransmission(4); // transmit to device #4
    Wire.write("12");
    Wire.endTransmission();
  }
}

void sendCharge(){
  if(batteryCharge == 0){
    irsend.sendNEC(hexForIR_howMuchCharge0, 32);
  }
  
  if(batteryCharge == 10){
    irsend.sendNEC(hexForIR_howMuchCharge1, 32); //Send til addresse og command.
  }
  
  if(batteryCharge == 20){
    irsend.sendNEC(hexForIR_howMuchCharge2, 32);
  }
  
  if(batteryCharge == 30){
    irsend.sendNEC(hexForIR_howMuchCharge3, 32);
  }

  if(batteryCharge == 40){
    irsend.sendNEC(hexForIR_howMuchCharge4, 32);
  }

  if(batteryCharge == 50){
    irsend.sendNEC(hexForIR_howMuchCharge5, 32);
  }

  if(batteryCharge == 60){
    irsend.sendNEC(hexForIR_howMuchCharge6, 32);
  }

  if(batteryCharge == 70){
    irsend.sendNEC(hexForIR_howMuchCharge7, 32);
  }

  if(batteryCharge == 80){
    irsend.sendNEC(hexForIR_howMuchCharge8, 32);
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

void transactionCase(){
  switch(transactionCaseNumber){
    case 0:
    batterygestures();

    break;

    case 1:
    //Avbryt
      doyouwanttocancel();
    break;

    case 2:
    //Kjøp
      doyouwanttobuy();
    break;

    case 3:
      sendCharge();
    break;
  }
}


void loop() {

  if (!client.connected()) {
        reconnect();
  }
  client.loop(); 

  //transactionCase();
  Serial.println(1);

}


