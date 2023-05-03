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
    int val = analogRead(pinIn);
    Serial.println(val);
    analogWrite(pinOut, val);
    previousMillis = currentMillis;
  }
}
