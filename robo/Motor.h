#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

class Motor {
  private:
  byte throttlePin, forwardPin, reversePin;
  int throttleAdjustment;
  byte throttle = 200;
  
  public:
  Motor();
  void attach(byte, byte, byte);
  void setThrottle(byte);
  void adjustThrottle(int);
  void forward();
  void reverse();
  void stop();
};

#endif
