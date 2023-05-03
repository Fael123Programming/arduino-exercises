int pinIn = 3, pinOut = 9;
unsigned long previousMillis = 0;
unsigned long interval = 50;

void setup() {
  Serial.begin(115200);
  pinMode(pinIn, INPUT_PULLUP);
  pinMode(pinOut, OUTPUT);
}

void f(float val) {
  analogWrite(pinOut, val);
}

void loop() {
  unsigned int currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    int analog = analogRead(pinIn);
    Serial.println(analog * (4 / 1023.0));
    f(analog);
    previousMillis = currentMillis;
  }
}
