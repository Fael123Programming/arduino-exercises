int pinIn = 3, pinOut = 9;
unsigned long previousMillis = 0;
unsigned long interval = 50;

void setup() {
  Serial.begin(115200);
  pinMode(pinIn, INPUT_PULLUP);
  pinMode(pinOut, OUTPUT);

}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    float voltage = analogRead(pinIn) * (4 / 1023.0);
    Serial.println(voltage);
    previousMilis = currentMillis;
  }
}
