#include "Motor.h"

Motor::Motor() {
}

void Motor::attach(byte throttlePin, byte forwardPin, byte reversePin) {
  this->throttlePin = throttlePin;
  this->forwardPin = forwardPin;
  this->reversePin = reversePin;
  pinMode(throttlePin, OUTPUT);
  pinMode(forwardPin, OUTPUT);
  pinMode(reversePin, OUTPUT);
}

void Motor::setThrottle(byte throttle) {
  analogWrite(throttlePin, throttle);
}

void Motor::forward() {
  digitalWrite(forwardPin, HIGH);
  digitalWrite(reversePin, LOW);
}

void Motor::reverse() {
  digitalWrite(forwardPin, LOW);
  digitalWrite(reversePin, HIGH);
}

void Motor::stop(){
  digitalWrite(forwardPin, LOW);
  digitalWrite(reversePin, LOW);
}

