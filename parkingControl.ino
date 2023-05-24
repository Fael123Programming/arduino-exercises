#include <Servo.h>

Servo servo1, servo2;

float d1, d2;

int servo1LastPos = 0, servo2LastPos = 0;
unsigned long limitMillis = 50;   
long servo1LastMillis = 0, servo2LastMillis = 0;

long readUltrasonicDistance(int triggerPin, int echoPin) {
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  return pulseIn(echoPin, HIGH);
}

void open(Servo s, int *servoLastPos, long *servoLastMillis) {
  long currentMillis = millis();
  for (; *servoLastPos <= 90; (*servoLastPos)++) {
    if (currentMillis - *servoLastMillis >= limitMillis) {
      s.write(*servoLastPos);
      *servoLastMillis = currentMillis;
    } else {
      break;
    }
  }
}

void close(Servo s, int *servoLastPos, long *servoLastMillis) {
  long currentMillis = millis();
  for (; *servoLastPos > -1; (*servoLastPos)--) {
    if (currentMillis - *servoLastMillis >= limitMillis) {
      s.write(*servoLastPos);
      *servoLastMillis = currentMillis;
    } else {
      break;
    }
  }
}

void setup() {
  Serial.begin(9600);
  servo1.attach(9);
  servo2.attach(10);
}

void loop() {
  d1 = 0.0344 / 2  * readUltrasonicDistance(3, 2);
  Serial.print("Distance servo1: ");
  Serial.println(d1);
  if (d1 < 30) {
    open(servo1, &servo1LastPos, &servo1LastMillis);
  } else {
    close(servo1, &servo1LastPos, &servo1LastMillis);
  }
  d2 = 0.0344 / 2  * readUltrasonicDistance(5, 4);
  Serial.print("Distance servo2: ");
  Serial.println(d2);
  if (d2 < 30) {
    open(servo2, &servo2LastPos, &servo2LastMillis);
  } else {
    close(servo2, &servo2LastPos, &servo2LastMillis);
  }
}

