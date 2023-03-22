#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* mqtt_server = "broker.hivemq.com";
const char* topicLed = "IPB/IoT/Class02/RafaelGuimaraes314883/LED";
const char* topicPot = "IPB/IoT/Class02/RafaelGuimaraes314883/Potentiometer";
unsigned int port = 1883;

const char* ssid = "agents";
const char* password = "QgC9O8VucAByqvVu5Rruv1zdpqM66cd23KG4ElV7vZiJND580bzYvaHqz5k07G2";

WiFiClient espClient;
PubSubClient client(espClient);

long lastTime = 0;
const int Msg_Size = 50;
char msg[Msg_Size];
int value = 0;

const char* read_pot() {
  int ar = analogRead(A0);
  char *ar_chars;
  sprintf(ar_chars, "%d", ar);
  return ar_chars;
}

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
  String strPayload="";
  for (int i = 0; i < length; i++) {
    strPayload += (char) payload[i];  
  }
  Serial.println("Message: ");
  Serial.println(strPayload);
  // The polarization of the led is reversed.
  if(strPayload[0] == "0") {
    digitalWrite(LED_BUILTIN, HIGH); //OFF 
  } else {
    digitalWrite(LED_BUILTIN, LOW); //ON 
  }
  Serial.println();
}

void reconnectBroker() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "AulaIoT-";
    clientId += String(random(0xffff), HEX);
    if(client.connect(clientId.c_str())) {
      Serial.println("Connected to the Broker");
      client.publish(read_pot(), topicPot);
      client.subscribe(topicLed); 
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
  while(WiFi.status() != WL_CONNECTED){ 
    WiFi.reconnect();
  }
  if(!client.connected()) {
    Serial.println("Node disconnected to Broker. Trying to reconnect... ");
    reconnectBroker();   
  }
  client.publish(read_pot(), topicPot);
  client.loop();
}
