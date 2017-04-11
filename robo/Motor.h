#ifndef MOTOR_H
#define MOTOR_H

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
  
  Motor(int throttlePin, int forwardPin, int backwardPin, int maxThrottle) {
    this->throttlePin = throttlePin;
    this->forwardPin = forwardPin;
    this->backwardPin = backwardPin;
    this->maxThrottle = maxThrottle;
    this->throttle = maxThrottle - 30;
    pinMode(throttlePin, OUTPUT);
    pinMode(forwardPin, OUTPUT);
    pinMode(backwardPin, OUTPUT);
  }

  void setThrottle(int throttle) {
    if (throttle != this->throttle) {
      this->throttle = throttle;
      analogWrite(throttlePin, constrain(throttle + throttleAdjustment, 0 ,maxThrottle));
    }
  }

  setThrottleAdjustment(int adjustment) {
    if (adjustment != throttleAdjustment) {
      throttleAdjustment = adjustment;
      analogWrite(throttlePin, constrain(throttle + throttleAdjustment, 0 ,maxThrottle));
    }
  }

  int getThrottle() {
    return throttle;
  }

  void setForwardPinState(byte state) {
     forwardPinState = state;
     digitalWrite(forwardPin, state);
  }

  void setBackwardPinState(byte state) {
     backwardPinState = state;
     digitalWrite(backwardPin, state);
  }

  void forward(int throttle) {
    setThrottle(throttle);
    setForwardPinState(HIGH);
    setBackwardPinState(LOW);
  }

  void backward(int throttle) {
    setThrottle(throttle);
    setForwardPinState(LOW);
    setBackwardPinState(HIGH);
  }

  void stop(){
    setThrottle(0);
    setForwardPinState(LOW);
    setBackwardPinState(LOW);
  }

  void pause() {
    paused = true;
    analogWrite(this->throttlePin, 0);
    digitalWrite(forwardPin, LOW);
    digitalWrite(backwardPin, LOW);
  }

  void resume() {
    if (paused) {
      analogWrite(this->throttlePin, throttle);
      digitalWrite(forwardPin, forwardPinState);
      digitalWrite(backwardPin, backwardPinState);
    }
  }

};

#endif

