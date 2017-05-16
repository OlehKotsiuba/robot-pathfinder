#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

class Motor {
  byte throttlePin, forwardPin, reversePin; 
  public:
  Motor();
  void attach(byte, byte, byte);
  void forward();
  void reverse();
  void setThrottle(byte);
  void stop();
};

#endif
