
#include <RF24.h>

#include "Message.h"
#include "Chassis.h"
#include "Locator.h"

#define SCAN_DELAY 100

RF24 radio(7,8);

byte addresses[][6] = {"1Node","2Node"};

Chassis *chassis;
Locator *locator;

enum RobotState {
  MOVE,
  DECISION
};

volatile RobotState state = DECISION;

long lastMessageTime = 0;

void onLeftInterrupt() {
  chassis->onLeftEncoderTick();
}

void onRightInterrupt() {
  chassis->onRightEncoderTick();
}

void setup() {
  radio.begin();

  radio.setPALevel(RF24_PA_LOW);

  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1,addresses[0]);

  radio.startListening();
    
  Motor *leftMotor = new Motor(6,4,0,200);
  Motor *rightMotor = new Motor(5,1,A0,200);

  chassis = new Chassis(leftMotor, rightMotor);
  chassis->setEncoderDataListener(sendEncoderData);

  attachInterrupt(digitalPinToInterrupt(2), onLeftInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), onRightInterrupt, CHANGE);

  locator = new Locator(10, A1, A2, 200, 10);
  //locator->setLocatorDataListener(sendLocatorData);
}

long lastScanTime = 0;

void loop()
{
  Message inRadiomessage;
  if(radio.available()) {
    while(radio.available()) {
      radio.read(&inRadiomessage,sizeof(Message));
    }
    processMessage(inRadiomessage);
  }

  int distances[5];
  if (chassis->isStopped()) {
    int maxDistance = 0;
    int maxDistanceAngle = 180;
    for (byte i = 0; i < 5; i++) {
      byte scanAngle = 45 * i;
      int distance = locator->scan(scanAngle);
      if (distance == -1) {
        maxDistanceAngle = scanAngle;
        maxDistance = 110;
        break;
      } 
      if (distance > maxDistance) {
        maxDistance = distance;
        maxDistanceAngle = scanAngle;
      }
    }
    if (maxDistance > 20) {
       if (maxDistanceAngle > 90) {
        chassis->turnLeft(maxDistanceAngle - 90);
        while(!chassis->tick());
        chassis->forward(maxDistance - 10);
      } else if (maxDistanceAngle < 90) {
        chassis->turnRight(90 - maxDistanceAngle);
        while(!chassis->tick());
        chassis->forward(maxDistance - 10);
      } else {
        chassis->forward(maxDistance - 10);
      }
    } else {
      chassis->turnLeft(90);
    }
  } else if (chassis->isMovingForward() && millis() - lastScanTime > SCAN_DELAY) {
    int distance = locator->scan(90);
    if (distance != -1 && distance < 20) {
      chassis->stop();
    }
    lastScanTime = millis();
  }
  chassis->tick();
}

void sendEncoderData(long leftCount, long rightCount) {
  Message message;
  message.type = ENCODER_DATA_MESSAGE;
  message.payloadA = leftCount + rightCount;
  message.payloadB = rightCount;
  radio.stopListening();
  //while(!radio.write(&message, sizeof(message)));
  radio.startListening();
}

void sendLocatorData(int position, int distance) {
  Message message;
  message.type = LOCATOR_DATA_MESSAGE;
  message.payloadA = position;
  message.payloadB = distance;
  radio.stopListening();
  //while(!radio.write(&message, sizeof(message)));
  radio.startListening();
}


void processMessage(Message msg) {
     switch(msg.type) {
      case MOVE_FORWARD_MESSAGE:
        chassis->forward();
      break;
      case MOVE_BACKWARD_MESSAGE:
        chassis->backward();
      break;
      case TURN_LEFT_MESSAGE:
        chassis->turnLeft();
      break;
      case TURN_RIGHT_MESSAGE:
        chassis->turnRight();
      break;
      case STOP_MESSAGE:
        chassis->stop();
      break;
    }
}

