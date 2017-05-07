#include "Locator.h"

int Locator::echo() {
  unsigned int impulseTime;
  unsigned int distance;
  noInterrupts();
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  impulseTime = pulseIn(echoPin, HIGH, TIMEOUT);
  interrupts();
  if (impulseTime == 0) distance = MAX_DISTANCE;
  else distance = impulseTime / 58;
  return distance;
}


Locator::Locator(){}

void Locator::rotate(int angle) {
  byte servoAngle = map(angle, 0, 180, MIN_ANGLE, MAX_ANGLE);
  servo.write(servoAngle);
  delay(abs(position - angle) * 4);
  position = angle;
}

void Locator::switchDirection() {
  if (direction == RIGHT) direction = LEFT;
  else direction = RIGHT;
}

int Locator::scan(int angle) {
  if (position != angle) {
    rotate(angle);
  }
  return echo();
}

void Locator::tick() {
  //(*locatorListener)(position, distance);
//  if (millis() - lastScanTime > scanDelay) {
//    scan();
//    lastScanTime = millis();
//  servo.write(position);
//    if (direction == LEFT ) {
//      position += scanWidth;
//      if (position >= MAX_ANGLE) switchDirection();
//    }
//    else {
//      position -= scanWidth;
//      if (position <= MIN_ANGLE) switchDirection();
//    }
//  }
}

void Locator::setLocatorDataListener(void(*listener)(int, int)) {
  locatorListener = listener;
}

void Locator::attachServo(byte pin) {
  servo.attach(pin);
  servo.write((MAX_ANGLE - MIN_ANGLE) / 2 + MIN_ANGLE);
}

void Locator::attachSonicSensor(byte echoPin, byte trigPin) {
  this->echoPin = echoPin;
  this->trigPin = trigPin;
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
}

