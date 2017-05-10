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
  leftMotor.setThrottle(throttle);
  rightMotor.setThrottle(throttle);
  leftEncoderCount = 0;
  rightEncoderCount = 0;
  targetPath = 0;
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
  int p = 20;
  int error = leftEncoderCount - rightEncoderCount;
  if (error != 0) {
    leftMotor.adjustThrottle(-error * p);
    rightMotor.adjustThrottle(error * p);
  } else {
    leftMotor.adjustThrottle(0);
    rightMotor.adjustThrottle(0);
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
  if (state != MOVING_FORWARD) {
    stateChanged();
    state = MOVING_FORWARD;
    leftMotor.forward();
    rightMotor.forward();
  }
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
  leftMotor.setThrottle(lThrottle);
  rightMotor.setThrottle(rThrottle);
  if (lThrottle >= 0) leftMotor.forward();
  else leftMotor.reverse();
  if (rThrottle >= 0) rightMotor.forward();
  else rightMotor.reverse();
}

void Chassis::backward() {
  if (state != MOVING_BACKWARD) {
    stateChanged();
    state = MOVING_BACKWARD;
    leftMotor.reverse();
    rightMotor.reverse();
  }
}

void Chassis::backward(int distance) {
  backward();
  targetPath = distance * TICKS_IN_CM;
}

void Chassis::turnLeft() { 
  if (state != TURNING_LEFT) {
    stateChanged();
    state = TURNING_LEFT;
    leftMotor.reverse();
    rightMotor.forward();
  }  
}

void Chassis::turnLeft(int angle) { 
  turnLeft();
  targetPath = angle * TICKS_IN_DEG;
}

void Chassis::turnRight() {
  if (state != TURNING_RIGHT) {
    stateChanged();
    state = TURNING_RIGHT;
    leftMotor.forward();
    rightMotor.reverse();
  }
}

void Chassis::turnRight(int angle) {
  turnRight();
  targetPath = angle * TICKS_IN_DEG;
}

void Chassis::turn(int angle) {
  if (angle > 0) turnLeft(angle);
  else turnRight(abs(angle));
}

void Chassis::stop() {
  if (state != STOP) {
  //brake();
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

void Chassis::brake() {
  long moveTime = millis() - startMoveTime;
  if (state == TURNING_LEFT) {
    leftMotor.forward();
    rightMotor.reverse();
  } else if (state == TURNING_RIGHT) {
    leftMotor.reverse();
    rightMotor.forward();
  } else if (state == MOVING_FORWARD) {
    leftMotor.reverse();
    rightMotor.reverse();
  } else if (state == MOVING_BACKWARD) {
    leftMotor.forward();
    rightMotor.forward();
  }
  delay(constrain(moveTime / 10, 0, 200));
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
  checkBalance();
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


