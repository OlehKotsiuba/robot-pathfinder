#include "Locator.h"

int Locator::echo() {
  unsigned int impulseTime;
  unsigned int distance;
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  impulseTime = pulseIn(echoPin, HIGH, TIMEOUT);
  if (impulseTime == 0) distance = MAX_DISTANCE;
  else distance = impulseTime / 58;
  return distance;
}

Locator::Locator(){}

void Locator::setLocatorDataListener(void(*listener)(int, int)) {
  locatorListener = listener;
}

void Locator::attach(byte echoPin, byte trigPin) {
  this->echoPin = echoPin;
  this->trigPin = trigPin;
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
}

