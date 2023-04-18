
int pinIn = 5, pinOut = 6;
int builtInLed = HIGH;
unsigned long previousMillis = 0;
unsigned long interval = 1000;

void setup() {
  Serial.begin(115200);
  pinMode(pinIn, INPUT_PULLUP);
  pinMode(pinOut, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    digitalWrite(LED_BUILTIN, builtInLed);
    if (builtInLed == HIGH) {
      builtInLed = LOW;
    } else {
      builtInLed = HIGH;
    }
    previousMillis = currentMillis;
    Serial.println(digitalRead(pinIn));
  }
  digitalWrite(pinOut, !digitalRead(pinIn));
}
