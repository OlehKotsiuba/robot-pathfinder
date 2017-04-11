#include <Servo.h>

#ifndef LOCATOR_H
#define LOCATOR_H

#define LEFT 0
#define RIGHT 1
#define TIMEOUT 5800
#define MAX_ANGLE 170
#define MIN_ANGLE 10

class Locator {
  private:
  int position = 90;
  byte direction = RIGHT;
  Servo servo;
  long lastScanTime;
  int scanDelay;
  byte scanWidth;

  void (*locatorListener)(int, int);

  byte servoPin;
  byte echoPin, trigPin;

  void scan() {
    unsigned int impulseTime;
    unsigned int distance;
    noInterrupts();
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    impulseTime = pulseIn(echoPin, HIGH, TIMEOUT);
    interrupts();
    if (impulseTime == 0) distance = TIMEOUT / 58;
    else distance = impulseTime / 58;
    (*locatorListener)(position, distance);
  }

  public:
  Locator(byte servoPin, byte echoPin, byte trigPin, int scanDelay, byte scanWidth):
  servoPin(servoPin), echoPin(echoPin), trigPin(trigPin), scanDelay(scanDelay), scanWidth(scanWidth){
    pinMode(echoPin, INPUT);
    pinMode(trigPin, OUTPUT);
    servo.attach(servoPin);
    servo.write(10);
  }
  
  void switchDirection() {
    if (direction == RIGHT) direction = LEFT;
    else direction = RIGHT;
  }
  
  void tick() {
    //servo.write(90);
   if (millis() - lastScanTime > scanDelay) {
     scan();
     lastScanTime = millis();
     servo.write(position);
     if (direction == LEFT ) {
      position += scanWidth;
      if (position >= MAX_ANGLE) switchDirection();
     }
     else {
      position -= scanWidth;
      if (position <= MIN_ANGLE) switchDirection();
     }
    }
  }



  setLocatorDataListener(void(*listener)(int, int)) {
    locatorListener = listener;
  }
};

#endif
