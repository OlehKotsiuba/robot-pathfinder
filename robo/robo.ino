
#include <RF24.h>
#include "I2Cdev.h"
#include "HMC5883L.h"
#include "Message.h"
#include "Chassis.h"
#include "Locator.h"
#include <Servo.h>


#define SCAN_DELAY 100
#define SAFE_DISTANCE 30
#define SCAN_ANGLE 90
#define SCAN_RESOLUTION 180 / SCAN_ANGLE

RF24 radio(7,8);
HMC5883L mag;

Servo servo;

byte addresses[][6] = {"1Node","2Node"};

Locator locator;
Chassis chassis;
int currentAngle = 0;

int previousError = 0;
long integralError = 0;

int targetHeading = 0;
int heading = 0;

struct Path {
  int angle;
  int distance;
};

enum Mode: byte {
  MANUAL = 0,
  AUTO = 1
} mode = MANUAL;

enum State: byte {
  SCANNING,
  MOVING,
  TURNING
} state = TURNING;

long lastMessageReceived = 0;
long lastScanTime = 0;

void sendEncoderData(long leftCount, long rightCount) {
  sendMessage(Message::ENCODER_DATA, leftCount, rightCount);
}

void sendLocatorData(int position, int distance) {
  sendMessage(Message::LOCATOR_DATA, position, distance);
}

void sendMessage(Message::Type type, int lowWord, int highWord) {
  Message message;
  message.payload.words.l = lowWord;
  message.payload.words.h = highWord;
  byte retry = 0;
  radio.stopListening();
  radio.write(&message, sizeof(message));
  radio.startListening();
}

void processMessage(Message *msg) {
  if (mode == MANUAL) processManualMoveMessage(msg);
  switch(msg->type) {
    case Message::SET_MODE:
      mode = msg->payload.dWord;
      targetHeading = getHeading();
    case Message::SET_HEADING:
      heading = abs(msg->payload.words.l);
    break;
  }
}

void processManualMoveMessage(Message *msg) {
  switch(msg->type) {
      case Message::MOVE_ANALOG:
        chassis.analog(msg->payload.words.l, msg->payload.words.h);
      break;
      case Message::MOVE_FORWARD:
        chassis.forward();
      break;
      case Message::MOVE_BACKWARD:
        chassis.backward();
      break;
      case Message::TURN_LEFT:
        chassis.turnLeft();
      break;
      case Message::TURN_RIGHT:
        chassis.turnRight();
      break;
      case Message::STOP:
        chassis.stop();
  }
}

void setup() {
//  Serial.begin(115200);
  radio.begin();
  radio.setPayloadSize(sizeof(Message)); // Default payload size is 32 bytes. If we dont specify it, unused bytes of message will be filled with zeros

  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1,addresses[0]);

  radio.startListening(); // Entering receiver mode

  Wire.begin();
  mag.initialize();
  
  chassis.setEncoderDataListener(sendEncoderData);
  chassis.attachLeftMotor(6, 4, 0);
  chassis.attachRightMotor(5, 1, A0);

  Chassis::attachInterrupts(2,3);

  //locator.attachServo(10);
  locator.attach(A3, A2);
//  locator->setLocatorDataListener(sendLocatorData);

  targetHeading = getHeading();
  heading = 0;
  servo.attach(10, 700, 2310);
}

int getHeading() {
  int mx, my, mz;
  mag.getHeading(&mx, &my, &mz);
  float heading = atan2(my, mx);
  if (heading < 0) heading += 2*PI;
  return heading * 180/PI;
}

long lastCheck = 0;
byte correctChecks = 0;

int getError(int heading, int target) {
  int error = target - heading;
  if (error > 180) {
    error -=360;
  } else if (error < -180) {
    error += 360;
  }
  return error;
}

int rotate(int direction, int angle) {
  direction += angle;
  if(direction > 360) direction -= 360;
  else if (direction < 0) direction += 360;
  return direction;
}

void loop() {
  Message inRadiomessage;
  if(radio.available()) { // If there is a received message in message buffer
    while(radio.available()) { 
      radio.read(&inRadiomessage,sizeof(Message)); // Get the most recent message from buffer
    }
    processMessage(&inRadiomessage);
    lastMessageReceived = millis();
  }
  
  switch(mode) {
    case MANUAL:
      if (millis() - lastMessageReceived > 500) chassis.stop();
      chassis.tick();
    break;
    case AUTO:
      switch(state) {

        case TURNING: {
          if (millis() - lastCheck > 10){
            lastCheck = millis();
            int currentHeading = getHeading();
            int error = getError(currentHeading, heading);
            byte absoluteError = abs(error);
            integralError += absoluteError;
            int throttle = absoluteError * 4 + (integralError)*0.05 + (absoluteError - previousError)*128;
            previousError = absoluteError;
            throttle = constrain(throttle, 0, 255);
            chassis.setThrottle(throttle);
            if (absoluteError > 2) {
                if (error < 0) {
                  chassis.turnLeft();
                }
                else {
                  chassis.turnRight();
                }
            } else {
              correctChecks++; 
              previousError = 0;
              integralError = 0;
              chassis.stop();
              if (correctChecks > 100) {
                correctChecks = 0;
                //state = SCANNING;
              }
            }
          }
          break;
        }

        case SCANNING: {
          byte scanAngle = 0;
          byte distances = 0xFF;
          
          int servoPos = servo.read();
          int scanStartTime = 0;
          servo.write(0);
          delay(abs(servoPos - 10) * 4);
          for (byte i = 0; i < 5; i++) {
              scanStartTime = millis();
              servo.write(i * 45);
              delay(200);
              if (locator.echo() < SAFE_DISTANCE) {
                  distances &= ~(1 << i);
               }
          }
          if ((distances & 0b00001110) == 0b00001110) {
            state = MOVING;
          } else if ((distances & 0b00000111) == 0b00000111) {
            heading = rotate(heading, 45);
            state = TURNING;
          } else if ((distances & 0b0011100) == 0b0011100) {
            heading = rotate(heading, -45);
            state = TURNING;
          } else {
            heading = rotate(heading, 90);
            state = TURNING;
          }
          break;
        }

        case MOVING: {
          if (servo.read() != 90) {
            servo.write(90);
            delay(500);
          }
          servo.write(90);
          chassis.forward();
          if (millis() - lastScanTime > 200) {
            lastScanTime = millis();
            if (locator.echo() < SAFE_DISTANCE) {
                 /* Obstacle is located. Stop moving and start scanning for new path */
                 chassis.stop();
                 state = SCANNING; 
                 break;
            }
          }
          break;
        }
    }   
    chassis.tick();
  }
  
}


