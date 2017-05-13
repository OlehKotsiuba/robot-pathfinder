#include <Arduino.h>
#include <Servo.h>

#define LEFT 0
#define RIGHT 1
#define TIMEOUT 7000
#define MAX_ANGLE 180
#define MIN_ANGLE 10
#define MAX_DISTANCE 110
#define SCAN_SHIFT 20

#define Swap(x,y); x ^= y; y ^= x; x ^=y;

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

  public:
  Locator();
  void tick();
  void rotate(int);
  int scan(int angle);
  void setLocatorDataListener(void(*listener)(int, int));
  void attachServo(byte);
  void attachSonicSensor(byte, byte);
};

