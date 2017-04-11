#include "Motor.h"

#ifndef CHASSIS_H
#define CHASSIS_H

enum State {
  STOP,
  MOVING_FORWARD,
  MOVING_BACKWARD,
  TURNING_LEFT,
  TURNING_RIGHT,
  MOVING_FORWARD_BY,
  MOVING_BACKWARD_BY,
  TURNING_LEFT_BY,
  TURNING_RIGHT_BY,
};

class Chassis {
  private:
  Motor *leftMotor, *rightMotor;
  void (*encoderListener)(long, long);
  volatile long leftEncoderCount = 0;
  volatile long rightEncoderCount = 0;
  unsigned long lastLoopTime = 0;
  int targetPath = 0;
  State state = STOP;
  State lastState = STOP;
  int lThrottle, rThrottle;

  void stateChanged() {
    if (encoderListener != NULL) {
      (*encoderListener)(leftEncoderCount, rightEncoderCount);
      reset();
    }
  }
  
  void reset() {
    leftEncoderCount = 0;
    rightEncoderCount = 0;
    targetPath = 0;
  }
  
  public:
  Chassis(Motor *lMotor, Motor *rMotor): leftMotor(lMotor), rightMotor(rMotor) {
    lastLoopTime = millis();
  }

  void onLeftEncoderTick() {
    if (state == STOP) return;
    leftEncoderCount++;
    checkPath();
    checkBalance();
  }

  void onRightEncoderTick() {
    if (state == STOP) return;
    rightEncoderCount++;
    checkPath();
    checkBalance();
  }

  void checkPath() {
     if(targetPath != 0) {
      if (leftEncoderCount + rightEncoderCount >= targetPath) {
        stop();
      }
    }
  }

  void checkBalance() {
    int p = 10;
    if (leftEncoderCount > rightEncoderCount) {
      int error = leftEncoderCount - rightEncoderCount;
      leftMotor->setThrottleAdjustment(-error * p);
      rightMotor->setThrottleAdjustment(error * p);
    } else if (rightEncoderCount > leftEncoderCount) {
      long error = rightEncoderCount - leftEncoderCount;
      rightMotor->setThrottleAdjustment(-error * p);
      leftMotor->setThrottleAdjustment(error * p);
    } else {
      leftMotor->setThrottleAdjustment(0);
      rightMotor->setThrottleAdjustment(0);
    }
  }

  void forward(int lThrottle, int rThrottle) {
    this->lThrottle = rThrottle; 
    this->lThrottle = rThrottle; 
    if (state != MOVING_FORWARD) {
      stateChanged();
      state = MOVING_FORWARD;
      leftMotor->forward(lThrottle);
      rightMotor->forward(rThrottle);
    }
  }

  void backward(int lThrottle, int rThrottle) {
    this->lThrottle = rThrottle; 
    this->lThrottle = rThrottle; 
    if (state != MOVING_BACKWARD) {
      stateChanged();
      state = MOVING_BACKWARD;
      leftMotor->backward(lThrottle);
      rightMotor->backward(rThrottle);
    }
  }

  void turnLeft(int lThrottle, int rThrottle) {
    this->lThrottle = rThrottle; 
    this->lThrottle = rThrottle; 
    if (state != TURNING_LEFT) {
      stateChanged();
      state = TURNING_LEFT;
      leftMotor->backward(lThrottle);
      rightMotor->forward(rThrottle);
    }  
  }

  void turnRight(int lThrottle, int rThrottle) {
    this->lThrottle = rThrottle; 
    this->lThrottle = rThrottle; 
    if (state != TURNING_RIGHT) {
      stateChanged();
      state = TURNING_RIGHT;
      leftMotor->forward(lThrottle);
      rightMotor->backward(rThrottle);
    }
  }

  void forwardBy(int path) {
    if (state != MOVING_FORWARD_BY) {
      stateChanged();
      targetPath = path;
      state = MOVING_FORWARD_BY;
      leftMotor->forward(rThrottle);
      rightMotor->forward(rThrottle);
    }
  }

  void backwardBy(int path) {
    if (state != MOVING_BACKWARD_BY) {
      stateChanged();
      targetPath = path;
      state = MOVING_BACKWARD_BY;
      leftMotor->backward(lThrottle);
      rightMotor->backward(rThrottle);
    }
  }

  void turnLeftBy(int path) {
    if (state != TURNING_LEFT_BY) {
      stateChanged();
      targetPath = path;
      state = TURNING_LEFT_BY;
      leftMotor->backward(lThrottle);
      rightMotor->forward(rThrottle);
    }  
  }

  void turnRightBy(int path) {
    if (state != TURNING_RIGHT_BY) {
      stateChanged();
      targetPath = path;
      state = TURNING_RIGHT_BY;
      leftMotor->forward(lThrottle);
      rightMotor->backward(rThrottle);
    }
  }

  void stop() {
    if (state != STOP) {
    stateChanged();
//    brake();
    state = STOP;
    leftMotor->stop();
    rightMotor->stop();  
    }
  }

  void brake() {
    if (state == TURNING_LEFT || state == TURNING_LEFT_BY) {
      leftMotor->forward(lThrottle);
      rightMotor->backward(rThrottle);
    } else if (state == TURNING_RIGHT || state == TURNING_RIGHT_BY) {
      leftMotor->backward(lThrottle);
      rightMotor->forward(rThrottle);
    } else if (state == MOVING_FORWARD || state == MOVING_FORWARD_BY) {
      leftMotor->backward(lThrottle);
      rightMotor->backward(lThrottle);
    } else if (state == MOVING_BACKWARD || state == MOVING_BACKWARD_BY) {
      leftMotor->forward(lThrottle);
      rightMotor->forward(rThrottle);
    }
    delay(40);
  }

  long getLeftEncoderCount() {
    return leftEncoderCount;
  }

   long getRightEncoderCount() {
    return rightEncoderCount;
  }

  onEncoderData(void (*listener)(int, int)) {
    encoderListener = listener;
  }

  void loop() {

  }
};

#endif

