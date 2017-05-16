#include "Motor.h"

class Chassis {
  enum State {
    STOP,
    MOVING_FORWARD,
    MOVING_BACKWARD,
    TURNING_LEFT,
    TURNING_RIGHT,
    ANALOG
  };

  volatile static long leftEncoderCount;
  volatile static long rightEncoderCount;
  Motor leftMotor, rightMotor;
  void (*encoderListener)(long, long) = NULL;
  long lastTickTime = 0;
  int targetPath = 0;
  State state = STOP;
  int lThrottle, rThrottle;
  unsigned long startMoveTime = 0;
  byte throttle = 200;
  int balance = 0;

  void stateChanged();
  void reset();
  void checkPath();
  void checkBalance();
  
  public:
  Chassis();
  void attachLeftMotor(byte, byte, byte);
  void attachRightMotor(byte, byte, byte);
  void forward();
  void backward();
  void forward(int);
  void backward(int);
  void analog(int, int);
  void turnLeft();
  void turnRight();
  void turn(int);
  void setThrottle(byte);
  void stop();
  void brake();
  bool isStopped();
  bool isMovingForward();
  void setEncoderDataListener(void (*listener)(int, int));
  void tick();

  static void attachInterrupts(byte, byte); 
  static void onLeftInterrupt();
  static void onRightInterrupt();
};

