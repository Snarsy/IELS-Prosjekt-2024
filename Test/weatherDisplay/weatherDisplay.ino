#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

const char *ssid = "NTNU-IOT";
const char *pass = "";
const char *mqtt_server = "10.25.18.134";
const char *MQTT_UN = "fosfix";
const char *MQTT_PW = "C4nn3ds0up3s321";

long lastMsg = 0;


WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi()
{
  delay(10);
  Serial.println("Try Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Wifi connected successfully");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());
}

void setup()
{
  Serial.begin(9600);
  setup_wifi();
  // MQTT-client detaljer
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void callback(String topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (topic == "trondheim/temperature")
  {
    Serial.print("Temperature: ");
    Serial.println(messageTemp);
  }
  else if (topic == "trondheim/wind")
  {
    Serial.print("Wind: ");
    Serial.println(messageTemp);
  }
  /*else if (topic == "trondheim/description")
  {
    Serial.print("Description: ");
    Serial.println(messageTemp);
  }*/
  Serial.println();
}
void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  
  client.loop();

  StaticJsonDocument<80> doc;
  char output[80];

  // Delay between sending messages
  long now = millis();
  if (now - lastMsg > 5000)
  {
    lastMsg = now;

    // int on = 1;

    // Legger til verdier i JSON-dokument
    // doc["p"] = on;

    // Gjør om JSON til seriell
    serializeJson(doc, output);
    // Serial.println(output);
    client.publish("trondheim/weather", output);
  }

}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("MQTT not connected... Trying to connect");

    // Lager client-ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    // Prøver å koble til
    if (client.connect(clientId.c_str(), MQTT_UN, MQTT_PW))
    {
      Serial.println("Connected!");
      client.subscribe("trondheim/temperature");
      client.subscribe("trondheim/wind");
      client.subscribe("trondheim/description");
    }
    else
    {
      Serial.print("Failed, code=");
      Serial.println(client.state());
      delay(5000); // Wait 5 seconds before retrying
    }
  }
}