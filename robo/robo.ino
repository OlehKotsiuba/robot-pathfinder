
#include <RF24.h>

#include "Message.h"

#include "Chassis.h"
#include "Motor.h"
#include "Locator.h"

RF24 radio(7,8);

byte addresses[][6] = {"1Node","2Node"};

Chassis *chassis;
Locator *locator;

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
    
  Motor *leftMotor = new Motor(6,4,0,130);
  Motor *rightMotor = new Motor(5,1,A0,130);

  chassis = new Chassis(leftMotor, rightMotor);
  chassis->onEncoderData(sendEncoderData);

  attachInterrupt(digitalPinToInterrupt(2), onLeftInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), onRightInterrupt, CHANGE);

  locator = new Locator(10, A1, A2, 200, 10);
  locator->setLocatorDataListener(sendLocatorData);

}

void loop()
{
  Message inRadiomessage;
  if(radio.available()) {
    while(radio.available()) {
      radio.read(&inRadiomessage,sizeof(Message));
    }
    processMessage(inRadiomessage);
  } 

  locator->tick();
}

void sendEncoderData(long leftCount, long rightCount) {
  Message message;
  message.type = ENCODER_DATA_MESSAGE;
  message.payloadA = leftCount;
  message.payloadB = rightCount;
  radio.stopListening();
  radio.write(&message, sizeof(message));
  radio.startListening();
}

void sendLocatorData(int position, int distance) {
  Message message;
  message.type = LOCATOR_DATA_MESSAGE;
  message.payloadA = position;
  message.payloadB = distance;
  radio.stopListening();
  radio.write(&message, sizeof(message));
  radio.startListening();
}

void processMessage(Message msg) {
     switch(msg.type) {
      case MOVE_FORWARD_MESSAGE:
        chassis->forward(msg.payloadA, msg.payloadB);
      break;
      case MOVE_BACKWARD_MESSAGE:
        chassis->backward(msg.payloadA, msg.payloadB);
      break;
      case TURN_LEFT_MESSAGE:
        chassis->turnLeft(msg.payloadA, msg.payloadB);
      break;
      case TURN_RIGHT_MESSAGE:
        chassis->turnRight(msg.payloadA, msg.payloadB);
      break;
      case MOVE_FORWARD_BY_MESSAGE:
        chassis->forwardBy(msg.payloadA);
      break;
      case MOVE_BACKWARD_BY_MESSAGE:
        chassis->backwardBy(msg.payloadA);
      break;
      case TURN_LEFT_BY_MESSAGE:
        chassis->turnLeftBy(msg.payloadA);
      break;
      case TURN_RIGHT_BY_MESSAGE:
        chassis->turnRightBy(msg.payloadA);
      break;
      case STOP_MESSAGE:
        chassis->stop();
      break;
    }
}

