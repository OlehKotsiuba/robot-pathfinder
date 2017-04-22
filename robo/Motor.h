#include <Arduino.h>

class Motor {
  
  private:
  byte throttlePin;
  byte forwardPin;
  byte backwardPin;
  byte maxThrottle;
  byte forwardPinState;
  byte backwardPinState;
  bool paused = false;
  int throttleAdjustment = 0;
    
  public:
  byte throttle;
  Motor(int throttlePin, int forwardPin, int backwardPin, int maxThrottle);
  void setThrottle(int throttle);
  void adjustThrottle(int adjustment);
  int getThrottle();
  void setForwardPinState(byte state);
  void setBackwardPinState(byte state);
  void forward();
  void backward();
  void stop();
  void pause();
  void resume();

};


