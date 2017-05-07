
#include <RF24.h>

#include "Message.h"
#include "Chassis.h"
#include "Locator.h"

#define SCAN_DELAY 100
#define SAFE_DISTANCE 20
#define SCAN_ANGLE 90
#define SCAN_RESOLUTION 180 / SCAN_ANGLE

RF24 radio(7,8);

byte addresses[][6] = {"1Node","2Node"};

Locator locator;
Chassis chassis;
int currentAngle = 0;

struct Path {
  int angle;
  int distance;
};

enum Mode: byte {
  MANUAL = 0,
  AUTO = 1
} mode = MANUAL;

enum State: byte {
  PATH_FINDING,
  MOVING
} state = PATH_FINDING;

long lastMessageReceived = 0;

void setup() {
  radio.begin();
  
  radio.setPayloadSize(sizeof(Message)); // Default payload size is 32 bytes. If we dont specify it, unused bytes of message will be filled with zeros

  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1,addresses[0]);

  radio.startListening(); // Entering receiver mode
  
  chassis.setEncoderDataListener(sendEncoderData);
  chassis.attachLeftMotor(6, 4, 0);
  chassis.attachRightMotor(5, 1, A0);

  Chassis::attachInterrupts(2,3);

  locator.attachServo(10);
  locator.attachSonicSensor(A3, A2);
//  locator->setLocatorDataListener(sendLocatorData);
}

Path findBestPath() {
  Path bestPath;
  unsigned int bestScore;
  for(byte i = 0; i <= 180 / SCAN_ANGLE ; i++) {
    int angle = i * SCAN_ANGLE;
    byte distance = locator.scan(angle);
    if (distance > SAFE_DISTANCE) { 
      int score = getPathScore(angle, distance);
      if (score > bestScore ) {
        bestPath.angle = angle;
        bestPath.distance = distance;
        bestScore = score;
      }
    }
  }
  return bestPath;
}

int getPathScore(int angle, int distance) {
  int direction = currentAngle + angle - 90;
    if (abs(direction) > 180) {
      direction %= 180;
      direction = direction > 0 ? 180 - direction : 180 + direction;
    }
  return (180 - abs(direction)) / 9 * distance;
}

bool tryMoveForward() {
  int distance = locator.scan(90);
  if (distance > SAFE_DISTANCE){
    chassis.forward(distance - SAFE_DISTANCE);
    return true;
  }
  return false;
}

bool tryTurnRight() {
  int distance = locator.scan(180);
   if (distance > SAFE_DISTANCE){
    chassis.turn(90);
    currentAngle += 90;
    if (currentAngle > 180) currentAngle =- 360;
    return true;
  }
  return false;
}

bool tryTurnLeft() {
  int distance = locator.scan(0);
   if (distance > SAFE_DISTANCE){
    chassis.turn(-90);
    currentAngle -= 90;
    if (currentAngle < -180) currentAngle =+ 360;
    return true;
  }
  return false;
}

bool rotate() {
  chassis.turn(180);
  currentAngle += 180;
  if (currentAngle > 180) -180 + currentAngle % 180;
}


void loop()
{
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
    break;
    case AUTO:
      if (chassis.isStopped()) {
        bool result = false;
        if (currentAngle > 0) {
           result = tryTurnLeft();
           if (!result) result = tryMoveForward();
           if (!result) result = tryTurnRight();
           if (!result) rotate();
         } else if (currentAngle < 0) {
           result = tryTurnRight();
           if (!result) result = tryMoveForward();
           if (!result) result = tryTurnLeft();
           if (!result) rotate();
         } else {
           result = tryMoveForward();
           if (!result) result = tryTurnRight();
           if (!result) result = tryTurnLeft();
           if (!result) rotate();
         }
      } else if (chassis.isMovingForward()) {
        if (locator.scan(90) < SAFE_DISTANCE) chassis.stop();
      }
    break;
  }
  chassis.tick();
}

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

