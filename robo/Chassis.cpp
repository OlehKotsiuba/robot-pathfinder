#include "Chassis.h"

#define TICKS_IN_CM 3.9
#define TICKS_IN_DEG 0.28

void Chassis::stateChanged() {
  if (encoderListener != NULL) {
    (*encoderListener)(leftEncoderCount, rightEncoderCount);
  }
  reset();
}

void Chassis::reset() {
  leftEncoderCount = 0;
  rightEncoderCount = 0;
  targetPath = 0;
  leftMotor.setThrottle(throttle);
  rightMotor.setThrottle(throttle);
  startMoveTime = millis();
}

Chassis::Chassis(){}

void Chassis::attachLeftMotor(byte throttlePin, byte forwardPin, byte reversePin) {
 leftMotor.attach(throttlePin, forwardPin, reversePin); 
}

void Chassis::attachRightMotor(byte throttlePin, byte forwardPin, byte reversePin) {
 rightMotor.attach(throttlePin, forwardPin, reversePin); 
}

void Chassis::checkBalance() {
  int error = leftEncoderCount - rightEncoderCount;
  if (error != 0) {
    balance = error * 20;
    leftMotor.setThrottle(constrain(throttle - balance, 0, 255));
    rightMotor.setThrottle(constrain(throttle + balance, 0, 255));
  } else {
    balance = 0;
  }
}

void Chassis::checkPath() {
  if (targetPath == 0) return;
  int path = rightEncoderCount + leftEncoderCount;
  if (path >= targetPath) {
    stop();
  }
}

void Chassis::forward() {
  if (state == MOVING_FORWARD) return;
  stateChanged();
  state = MOVING_FORWARD;
  leftMotor.forward();
  rightMotor.forward();
}

void Chassis::forward(int distance) {
  forward();
  targetPath = distance * TICKS_IN_CM;
}

void Chassis::analog(int lThrottle, int rThrottle) {
  if (state != ANALOG) {
    state = ANALOG;
    stateChanged();
  }
  leftMotor.setThrottle(abs(lThrottle));
  rightMotor.setThrottle(abs(rThrottle));
  if (lThrottle >= 0) leftMotor.forward();
  else leftMotor.reverse();
  if (rThrottle >= 0) rightMotor.forward();
  else rightMotor.reverse();
}

void Chassis::backward() {
  if (state == MOVING_BACKWARD) return;
  stateChanged();
  state = MOVING_BACKWARD;  
  leftMotor.reverse();
  rightMotor.reverse();
}

void Chassis::backward(int distance) {
  backward();
  targetPath = distance * TICKS_IN_CM;
}

void Chassis::turnLeft() {
  if (state == TURNING_LEFT) return;
  stateChanged();
  state = TURNING_LEFT;
  leftMotor.reverse();
  rightMotor.forward();
}


void Chassis::turnRight() {
  if (state != TURNING_RIGHT) 
  stateChanged();
  state = TURNING_RIGHT;
  leftMotor.forward();
  rightMotor.reverse();
}

void Chassis::stop() {
  if (state != STOP) {
  stateChanged();
  state = STOP;
  leftMotor.stop();
  rightMotor.stop();  
  }
}

void Chassis::setThrottle(byte throttle) {
  this->throttle = throttle;
  leftMotor.setThrottle(throttle);
  rightMotor.setThrottle(throttle);
}

void Chassis::setEncoderDataListener(void (*listener)(int, int)) {
  encoderListener = listener;
}

bool Chassis::isStopped() {
  return state == STOP;
}

bool Chassis::isMovingForward() {
  return state == MOVING_FORWARD;
}

void Chassis::tick() {
  if (state == ANALOG || state == STOP) return;
  //checkBalance();
  checkPath();
  return;
}

void Chassis::attachInterrupts(byte leftPin, byte rightPin) {
  attachInterrupt(digitalPinToInterrupt(2), onLeftInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), onRightInterrupt, CHANGE);
}

void Chassis::onLeftInterrupt() {
  leftEncoderCount++;
}

void Chassis::onRightInterrupt() {
  rightEncoderCount++;
}

volatile long Chassis::leftEncoderCount = 0;
volatile long Chassis::rightEncoderCount = 0;


