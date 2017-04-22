#include <Arduino.h>
#include <Servo.h>

#define LEFT 0
#define RIGHT 1
#define TIMEOUT 7000
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
  int echo();
  void switchDirection();
  
  public:
  Locator(byte servoPin, byte echoPin, byte trigPin, int scanDelay, byte scanWidth);
  void tick();
  int scan(int angle);
  void setLocatorDataListener(void(*listener)(int, int));
  
};

