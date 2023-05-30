#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <string>

using namespace std;

const char* mqtt_server = "broker.mqtt-dashboard.com";
const char* temperatureTopic = "healthMonitoring/patient1/temperature";
const char* systolicTopic = "healthMonitoring/patient1/bloodPressure/systolic";
const char* diastolicTopic = "healthMonitoring/patient1/bloodPressure/diastolic";
const char* heartRateTopic = "healthMonitoring/patient1/heartRate";
const char* oxigenPercentageTopic = "healthMonitoring/patient1/oxigenPercentage";

const unsigned int port = 1883;

const char* ssid = "agents";
const char* password = "QgC9O8/V/ucAByqvVu5Rruv1zdpqM66cd23KG4ElV7vZiJND580bzYvaHqz5k07G2";
//const char* ssid = "S ROQUE";/
//const char* password = "s@oR0que2021.";/

unsigned long currentMillis = 0, lastMillis = 0;
const unsigned long limit = 5000;

float temp, sysVal, diasVal;
unsigned int hr, op;  // Heart rate and oxigen percentage.

const float seedRate = 0.01;

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

string encrypt_int(int i) {
  string index[] = {"H", "q", "x", "(", "*", "=", "m", "'", "G", ">"};
  string encrypted = "", i_str = to_string(i);
  for (int j = 0; j < i_str.size(); j++) {
    encrypted += index[i_str[j] - '0'];
  }
  return encrypted;
}

string encrypt_float(float f) {
  string index[] = {"H", "q", "x", "(", "*", "=", "m", "'", "G", ">"};
  const string dot_symb = "Q";
  string encrypted = "", f_str = to_string(f);
  for (int j = 0; j < f_str.size(); j++) {
    if (f_str[j] == '.') 
      encrypted += dot_symb;
    else 
      encrypted += index[f_str[j] - '0'];
  }
  return encrypted;
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
    client.publish(temperatureTopic, encrypt_float(temp).c_str());
    client.publish(systolicTopic, encrypt_float(sysVal).c_str());
    client.publish(diastolicTopic, encrypt_float(diasVal).c_str());
    client.publish(heartRateTopic, encrypt_int(hr).c_str());
    client.publish(oxigenPercentageTopic, encrypt_int(op).c_str());
    lastMillis = currentMillis;
    temp = getRandomFloat(temp * (1.00 - seedRate), temp * (1.00 + seedRate));
    sysVal = getRandomFloat(sysVal * (1.00 - seedRate), sysVal * (1.00 + seedRate));
    diasVal = getRandomFloat(diasVal * (1.00 - seedRate), diasVal * (1.00 + seedRate));
    hr = getRandomInteger(hr * (1.00 - seedRate), hr * (1.00 + seedRate));
    op = getRandomInteger(op * (1.00 - seedRate), op * (1.00 + seedRate));
    if (temp > TEMP_MAX || temp < TEMP_MIN) {
      if (getRandomInteger(0, 1)) {
        temp = getRandomFloat(TEMP_MIN, TEMP_MAX);
      }
    }
    if (sysVal > SYS_MAX || sysVal < SYS_MIN) {
      if (getRandomInteger(0, 1)) {
        sysVal = getRandomFloat(SYS_MIN, SYS_MAX);
      }
    }
    if (diasVal > DIAS_MAX || diasVal < DIAS_MIN) {
      if (getRandomInteger(0, 1)) {
        diasVal = getRandomFloat(DIAS_MIN, DIAS_MAX);
      }
    }
    if (hr > HEART_RATE_MAX || hr < HEART_RATE_MIN) {
      if (getRandomInteger(0, 1)) {
        hr = getRandomInteger(HEART_RATE_MIN, HEART_RATE_MAX);
      }
    }
    if (op > OXIGEN_PERCENTAGE_MAX || op < OXIGEN_PERCENTAGE_MIN) {
      if (getRandomInteger(0, 1)) {
        op = getRandomInteger(OXIGEN_PERCENTAGE_MIN, OXIGEN_PERCENTAGE_MAX);
      }
    }
  }
}
