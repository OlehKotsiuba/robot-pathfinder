#include "Motor.h"

enum State {
  STOP,
  MOVING_FORWARD,
  MOVING_BACKWARD,
  TURNING_LEFT,
  TURNING_RIGHT
};

class Chassis {
  private:
  Motor *leftMotor, *rightMotor;
  void (*encoderListener)(long, long);
  void (*finishMoveListener)();
  void (*errorListener)(long);
  volatile long leftEncoderCount = 0;
  volatile long rightEncoderCount = 0;
  long lastTickTime = 0;
  int targetPath = 0;
  State state = STOP;
  int lThrottle, rThrottle;
  unsigned long startMoveTime = 0;

  void stateChanged();
  void reset();
  void checkPath();
  void checkBalance();
  
  public:
  Chassis(Motor *lMotor, Motor *rMotor);
  void onLeftEncoderTick();
  void onRightEncoderTick();

  void forward();
  void backward();
  void forward(int);
  void backward(int);
  void turnLeft();
  void turnRight();
  void turnLeft(int);
  void turnRight(int);
  void stop();
  void brake();
  bool isStopped();
  bool isMovingForward();
  long getLeftEncoderCount();
  long getRightEncoderCount();
  void setEncoderDataListener(void (*listener)(int, int));
  void setFinishMoveListener(void (*listener)());
  bool tick();
};

