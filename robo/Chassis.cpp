#include "Chassis.h"

#define TICKS_IN_CM 3.9
#define TICKS_IN_DEG 0.75

void Chassis::stateChanged() {
  if (encoderListener != NULL) {
    (*encoderListener)(leftEncoderCount, rightEncoderCount);
    reset();
  }
}

void Chassis::reset() {
  leftEncoderCount = 0;
  rightEncoderCount = 0;
  targetPath = 0;
  startMoveTime = millis();
}

Chassis::Chassis(Motor *lMotor, Motor *rMotor): leftMotor(lMotor), rightMotor(rMotor) {}

void Chassis::onLeftEncoderTick() {
  if (state == STOP) return;
  leftEncoderCount++;
}

void Chassis::onRightEncoderTick() {
  if (state == STOP) return;
  rightEncoderCount++;
}

void Chassis::checkBalance() {
  int p = 10;
  int error = leftEncoderCount - rightEncoderCount;
  if (error != 0) {
    leftMotor->adjustThrottle(-error * p);
    rightMotor->adjustThrottle(error * p);
  } else {
    leftMotor->adjustThrottle(0);
    rightMotor->adjustThrottle(0);
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
    leftMotor->forward();
    rightMotor->forward();
  }
}

void Chassis::forward(int distance) {
  forward();
  targetPath = distance * TICKS_IN_CM;
}

void Chassis::backward() {
  if (state != MOVING_BACKWARD) {
    stateChanged();
    state = MOVING_BACKWARD;
    leftMotor->backward();
    rightMotor->backward();
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
    leftMotor->backward();
    rightMotor->forward();
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
    leftMotor->forward();
    rightMotor->backward();
  }
}

void Chassis::turnRight(int angle) {
  turnRight();
  targetPath = angle * TICKS_IN_DEG;
}

void Chassis::stop() {
  if (state != STOP) {
  brake();
  stateChanged();
  state = STOP;
  leftMotor->stop();
  rightMotor->stop();  
  }
}

void Chassis::brake() {
  long moveTime = millis() - startMoveTime;
  if (state == TURNING_LEFT) {
    leftMotor->forward();
    rightMotor->backward();
  } else if (state == TURNING_RIGHT) {
    leftMotor->backward();
    rightMotor->forward();
  } else if (state == MOVING_FORWARD) {
    leftMotor->backward();
    rightMotor->backward();
  } else if (state == MOVING_BACKWARD) {
    leftMotor->forward();
    rightMotor->forward();
  }
  delay(constrain(moveTime / 10, 0, 200));
}

long Chassis::getLeftEncoderCount() {
  return leftEncoderCount;
}

 long Chassis::getRightEncoderCount() {
  return rightEncoderCount;
}

void Chassis::setEncoderDataListener(void (*listener)(int, int)) {
  encoderListener = listener;
}

void Chassis::setFinishMoveListener(void (*listener)()) {
  finishMoveListener = listener;
}

bool Chassis::isStopped() {
  return state == STOP;
}

bool Chassis::isMovingForward() {
  return state == MOVING_FORWARD;
}

bool Chassis::tick() {
  checkBalance();
  checkPath();
  return (state == STOP);
}


