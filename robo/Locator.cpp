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
  if (impulseTime == 0) distance = -1;
  else distance = impulseTime / 58;
  return distance;
}


Locator::Locator(byte servoPin, byte echoPin, byte trigPin, int scanDelay, byte scanWidth):
servoPin(servoPin), echoPin(echoPin), trigPin(trigPin), scanDelay(scanDelay), scanWidth(scanWidth){
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  servo.attach(servoPin);
  servo.write(90);
}

void Locator::switchDirection() {
  if (direction == RIGHT) direction = LEFT;
  else direction = RIGHT;
}

int Locator::scan(int angle) {
  if (position != angle) {
    servo.write(angle);
    delay(abs(position - angle) * 4);
    position = angle;
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


