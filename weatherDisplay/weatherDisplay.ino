#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

const char* ssid = "NTNU-IOT";
const char* pass = "";
const char* mqtt_server = "10.25.19.42";
const char* MQTT_UN = "fosfix";
const char* MQTT_PW = "C4nn3ds0up3s321";

const int potPin = 34;

long lastMsg = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  delay(100);
  Serial.begin(9600);

  Serial.println("Try Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Wifi connected successfully");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());

  //MQTT-client detaljer
  client.setServer(mqtt_server, 1883);

}

void loop() {
  if(!client.connected()){
    reconnect();
  }

  StaticJsonDocument<80> doc;
  char output[80];

  //Delay between sending messages
  long now = millis();
  if(now - lastMsg > 100){
    lastMsg = now;

    float pot = analogRead(potPin);

    //Legger til verdier i JSON-dokument
    doc["p"] = pot;

    //Gjør om JSON til seriell
    serializeJson(doc, output);
    Serial.println(output);
    client.publish("/test/sensor", output);
  }

}

void reconnect(){
  while(!client.connected()){
    Serial.print("MQTT not connected... Trying to connect");

    //Lager client-ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    //Prøver å koble til
    if(client.connect(clientId.c_str(), MQTT_UN, MQTT_PW)){
      Serial.println("Connected!");
    }
    else{
      Serial.print("Failed, code=");
      Serial.println(client.state());
      delay(5000); // Wait 5 seconds before retrying
    }
  }
}