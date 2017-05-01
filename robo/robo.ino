
#include <RF24.h>

#include "Message.h"
#include "Chassis.h"
#include "Locator.h"

#define SCAN_DELAY 100
#define SAFE_DISTANCE 20
#define SCAN_ANGLE 45
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

enum RobotState {
  MOVE,
  IDLE
};

volatile RobotState state = IDLE;

long lastMessageTime = 0;

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
  locator.attachSonicSensor( A1, A2);
  //locator->setLocatorDataListener(sendLocatorData);
}

Path findBestPath() {
  Path bestPath;
  byte bestScore;
  for(byte i = 0; i <= 180 / SCAN_ANGLE ; i++) {
    int angle = i * SCAN_ANGLE - 90;
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
  int direction = currentAngle + angle;
    if (abs(direction) > 180) {
      direction %= 180;
      direction = direction > 0 ? 180 - direction : 180 + direction;
    }
  return (180 - abs(direction)) / 9 * distance;
}

void loop()
{
  Message inRadiomessage;
  if(radio.available()) { // If there is a received message in message buffer
    while(radio.available()) { 
      radio.read(&inRadiomessage,sizeof(Message)); // Get the most recent message from buffer
    }
    processMessage(inRadiomessage);
  }

//  if (chassis.isStopped()) {
//    Path bestPath = findBestPath();
//    chassis.turn(bestPath.angle);
//    currentAngle += bestPath.angle;
//    while(!chassis.tick());
//    chassis.forward(bestPath.distance );  
//  } else if (chassis.isMovingForward()) {
//    if (locator.scan(0) > SAFE_DISTANCE) chassis.stop();
//  }
//  chassis.tick();
}



void sendEncoderData(long leftCount, long rightCount) {
  sendMessage(ENCODER_DATA_MESSAGE, leftCount, rightCount);
}

void sendLocatorData(int position, int distance) {
  sendMessage(LOCATOR_DATA_MESSAGE, position, distance);
}

void sendMessage(MessageType type, int payloadA, int payloadB) {
  Message message;
  message.payloadA = payloadA;
  message.payloadB = payloadB;
  byte retry = 0;
  radio.stopListening();
  radio.write(&message, sizeof(message));
  radio.startListening();
}

void processMessage(Message msg) {
     switch(msg.type) {
      case MOVE_FORWARD_MESSAGE:
        chassis.forward();
      break;
      case MOVE_BACKWARD_MESSAGE:
        chassis.backward();
      break;
      case TURN_LEFT_MESSAGE:
        chassis.turnLeft();
      break;
      case TURN_RIGHT_MESSAGE:
        chassis.turnRight();
      break;
      case STOP_MESSAGE:
        chassis.stop();
      break;
    }
}

