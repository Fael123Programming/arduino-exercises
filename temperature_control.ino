#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Broker configuration.
const char* mqtt_server = "broker.mqtt-dashboard.com";
const char* topic = "what/a/great/day";
unsigned int port = 1883;

// Wi-fi configuration.
const char* ssid = "agents";
const char* password = "QgC9O8VucAByqvVu5Rruv1zdpqM66cd23KG4ElV7vZiJND580bzYvaHqz5k07G2";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup_wifi() {
  Serial.println("Starting Setup Wifi");
  delay(10);
  Serial.println();
  Serial.print("Connecting to Wifi ");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
}

void callback(String top, byte* payload, unsigned int length) {
  Serial.print("Topic: ");
  Serial.print(top);
  String strPayload="";  // Keep the window condition.
  for (int i = 0; i < length; i++) {
    strPayload += (char) payload[i];  
  }
  Serial.println("Message: ");
  Serial.println(strPayload);
  // LED is ON: window is OPEN;
  // LED is OFF: window is CLOSED.
  if(strPayload == "CLOSED") {
    digitalWrite(LED_BUILTIN, HIGH); //OFF 
  } else if (strPayload == "OPEN") {
    digitalWrite(LED_BUILTIN, LOW); //ON 
  }
  Serial.println();
}

void reconnectBroker() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "Temperature control-";
    clientId += String(random(0xffff), HEX);
    if(client.connect(clientId.c_str())) {
      Serial.println("Connected to the Broker");
      client.subscribe(topic); 
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println("Trying again in 5 seconds...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.println("\n Starting Setup...");
  setup_wifi();
  client.setServer(mqtt_server, port);
  client.setCallback(callback);  
}


void loop() {
  // Check Internet connection to stay always connected.
  while(WiFi.status() != WL_CONNECTED){ 
    WiFi.reconnect();
  }
  // Check broker connection to stay always connected.
  if(!client.connected()) {
    Serial.println("Node disconnected to Broker. Trying to reconnect... ");
    reconnectBroker();   
  }
  client.loop();
}
