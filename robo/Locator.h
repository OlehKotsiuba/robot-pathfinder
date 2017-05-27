#include <Arduino.h>

#define LEFT 0
#define RIGHT 1
#define TIMEOUT 7000
#define MAX_DISTANCE 110

class Locator {
  private:
  void (*locatorListener)(int, int);
  byte echoPin, trigPin;

  public:
  Locator();
  int echo();
  void setLocatorDataListener(void(*listener)(int, int));
  void attach(byte, byte);
};

