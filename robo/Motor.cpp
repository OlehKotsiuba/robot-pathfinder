#include "Motor.h"

Motor::Motor(int throttlePin, int forwardPin, int backwardPin, int maxThrottle) {
  this->throttlePin = throttlePin;
  this->forwardPin = forwardPin;
  this->backwardPin = backwardPin;
  this->maxThrottle = maxThrottle;
  setThrottle(maxThrottle * 0.7);
  pinMode(throttlePin, OUTPUT);
  pinMode(forwardPin, OUTPUT);
  pinMode(backwardPin, OUTPUT);
}

void Motor::setThrottle(int throttle) {
  this->throttle = throttle;
  analogWrite(throttlePin, constrain(throttle + throttleAdjustment, 0 ,maxThrottle));
}

void Motor::adjustThrottle(int adjustment) {
  throttleAdjustment = adjustment;
  analogWrite(throttlePin, constrain(throttle + throttleAdjustment, 0 ,maxThrottle));
}

int Motor::getThrottle() {
  return throttle;
}

void Motor::forward() {
  digitalWrite(forwardPin, HIGH);
  digitalWrite(backwardPin, LOW);
}

void Motor::backward() {
  digitalWrite(forwardPin, LOW);
  digitalWrite(backwardPin, HIGH);
}

void Motor::stop(){
  digitalWrite(forwardPin, LOW);
  digitalWrite(backwardPin, LOW);
}

