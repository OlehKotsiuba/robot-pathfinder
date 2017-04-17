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
  volatile static long leftEncoderCount;
  volatile static long rightEncoderCount;
  Motor *leftMotor, *rightMotor;
  void (*encoderListener)(long, long) = NULL;
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
  Chassis(byte, byte, byte, byte, byte, byte, byte);
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
  void setEncoderDataListener(void (*listener)(int, int));
  bool tick();

  static void attachInterrupts(byte, byte); 
  static void onLeftInterrupt();
  static void onRightInterrupt();
};

