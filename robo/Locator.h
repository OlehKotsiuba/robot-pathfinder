#include <Arduino.h>
#include <Servo.h>

#define LEFT 0
#define RIGHT 1
#define TIMEOUT 7000
#define MAX_ANGLE 150
#define MIN_ANGLE 17

class Locator {
  private:
  int position = 0;
  byte direction = RIGHT;
  Servo servo;
  long lastScanTime;
  void (*locatorListener)(int, int);
  byte echoPin, trigPin;
  int echo();
  void switchDirection();
  void rotate(int);
  public:
  Locator();
  void tick();
  int scan(int angle);
  void setLocatorDataListener(void(*listener)(int, int));
  void attachServo(byte);
  void attachSonicSensor(byte, byte);
};

