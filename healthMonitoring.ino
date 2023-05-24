#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

using namespace std;

const char* mqtt_server = "broker.mqtt-dashboard.com";
const char* temperatureTopic = "HealthMonitoring/Temperature";
const char* bloodPressureTopic = "HealthMonitoring/BloodPressure";
const char* heartRateTopic = "HealthMonitoring/HeartRate";
const char* oxigenPercentageTopic = "HealthMonitoring/OxigenPercentage";

const unsigned int port = 1883;

const char* ssid = "agents";
const char* password = "QgC9O8VucAByqvVu5Rruv1zdpqM66cd23KG4ElV7vZiJND580bzYvaHqz5k07G2";

unsigned long currentMillis = 0, lastMillis = 0;
const unsigned long limit = 5000;

float temp, sysVal, diasVal;
unsigned int hr, op;  // Heart rate and oxigen percentage.

const float seedRate = 0.1;

const unsigned int TEMP_MAX = 40, TEMP_MIN = 35;
const unsigned int SYS_MAX = 180, SYS_MIN = 90;
const unsigned int DIAS_MAX = 120, DIAS_MIN = 60;
const unsigned int HEART_RATE_MAX = 250, HEART_RATE_MIN = 40;
const unsigned int OXIGEN_PERCENTAGE_MAX = 100, OXIGEN_PERCENTAGE_MIN = 80;

WiFiClient espClient;
PubSubClient client(espClient);

long lastTime = 0;
const int Msg_Size = 50;
char msg[Msg_Size];
int value = 0;

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

void reconnectBroker() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "AulaIoT-";
    clientId += String(random(0xffff), HEX);
    if(client.connect(clientId.c_str())) {
      Serial.println("Connected to the Broker"); 
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println("Trying again in 5 seconds...");
      delay(5000);
    }
  }
}

int getRandomInteger(int min, int max) {
  static bool first = true;
  if (first) {
    std::srand(time(NULL));
    first = false;
  }
  return min + std::rand() %  (max - min + 1);
}

float getRandomFloat(int min, int max) {
  static bool first = true;
  if (first) {
    std::srand(time(NULL));
    first = false;
  }
  return min + ((float) std::rand()) / (float) RAND_MAX * (max - min);
}

void setup() {
  Serial.begin(9600);
  Serial.println("\nStarting Setup...");
  setup_wifi();
  client.setServer(mqtt_server, port);
  temp = getRandomFloat(TEMP_MIN, TEMP_MAX);
  sysVal = getRandomFloat(SYS_MIN, SYS_MAX);
  diasVal = getRandomFloat(DIAS_MIN, DIAS_MAX);
  hr = getRandomInteger(HEART_RATE_MIN, HEART_RATE_MAX);
  op = getRandomInteger(OXIGEN_PERCENTAGE_MIN, OXIGEN_PERCENTAGE_MAX);
  Serial.print("Temperature seed: ");
  Serial.println(temp);
  Serial.print("Systolic seed: ");
  Serial.println(sysVal);
  Serial.print("Diastolic seed: ");
  Serial.println(diasVal);
  Serial.print("Heart rate seed: ");
  Serial.println(hr);
  Serial.print("Oxigen percentage seed: ");
  Serial.println(op);
}


void loop() {
  while(WiFi.status() != WL_CONNECTED){ 
    WiFi.reconnect();
  }
  if(!client.connected()) {
    Serial.println("Node disconnected to Broker. Trying to reconnect... ");
    reconnectBroker();   
  }
  currentMillis = millis();
  if (currentMillis - lastMillis >= limit) {
    string temperature = std::to_string(temp);
    string bloodPressure = std::to_string(sysVal) + "," + std::to_string(diasVal);
    string heartRate = std::to_string(hr);
    string oxigenPercentage = std::to_string(op);
    client.publish(temperatureTopic, temperature.c_str());
    client.publish(bloodPressureTopic, bloodPressure.c_str());
    client.publish(heartRateTopic, heartRate.c_str());
    client.publish(oxigenPercentageTopic, oxigenPercentage.c_str());
    lastMillis = currentMillis;
    temp = getRandomFloat(temp * (1.00 - seedRate), temp * (1.00 + seedRate));
    sysVal = getRandomFloat(sysVal * (1.00 - seedRate), sysVal * (1.00 + seedRate));
    diasVal = getRandomFloat(diasVal * (1.00 - seedRate), diasVal * (1.00 + seedRate));
    hr = getRandomInteger(hr * (1.00 - seedRate), hr * (1.00 + seedRate));
    op = getRandomInteger(op * (1.00 - seedRate), op * (1.00 + seedRate));
    if (temp > TEMP_MAX || temp < TEMP_MIN) {
      temp = getRandomFloat(TEMP_MIN, TEMP_MAX);
    }
    if (sysVal > SYS_MAX || sysVal < SYS_MIN) {
      sysVal = getRandomFloat(SYS_MIN, SYS_MAX);
    }
    if (diasVal > DIAS_MAX || diasVal < DIAS_MIN) {
      diasVal = getRandomFloat(DIAS_MIN, DIAS_MAX);
    }
    if (hr > HEART_RATE_MAX || hr < HEART_RATE_MIN) {
      hr = getRandomInteger(HEART_RATE_MIN, HEART_RATE_MAX);
    }
    if (op > OXIGEN_PERCENTAGE_MAX || op < OXIGEN_PERCENTAGE_MIN) {
      op = getRandomInteger(OXIGEN_PERCENTAGE_MIN, OXIGEN_PERCENTAGE_MAX);
    }
  }
}
