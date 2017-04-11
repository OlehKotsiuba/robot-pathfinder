#include <SPI.h>
#include "RF24.h"
#include "Message.h"

#define X_AXIS_PIN A0
#define Y_AXIS_PIN A1

RF24 radio(7,8);

byte addresses[][6] = {"1Node", "2Node"};

void setup() {
  Serial.begin(115200);
  
  radio.begin();

  radio.setPALevel(RF24_PA_LOW);
  
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1,addresses[1]);

  radio.stopListening();
}

void loop() {

   if(radio.available()) {
      Message inRadioMessage;   
      radio.read(&inRadioMessage,sizeof(Message));
      switch (inRadioMessage.type) {
        case ENCODER_DATA_MESSAGE: 
          Serial.print("E,");
          Serial.print(inRadioMessage.payloadA);
          Serial.print(",");
          Serial.println(inRadioMessage.payloadB); 
        break;
        case LOCATOR_DATA_MESSAGE:
          Serial.print("l,");
          Serial.print(inRadioMessage.payloadA);
          Serial.print(",");
          Serial.println(inRadioMessage.payloadB); 
        break;
      }
   }
  
  if(Serial.available()) {
    char serialInMsg[20];
    byte length = Serial.readBytesUntil('\n', serialInMsg, sizeof(serialInMsg));
    serialInMsg[length] = '\0';
    Message outRadioMessage = getMessageFromSerial(serialInMsg);
    radio.stopListening();
    radio.write( &outRadioMessage, sizeof(Message));
  }

  radio.startListening();
}

Message getMessageFromSerial(char * serialInMsg) {
   Message msg;
   char *msgType = strtok(serialInMsg, ",");
    switch(msgType[0]) {
      case 'F': {
        msg.type = MOVE_FORWARD_MESSAGE;
        msg.payloadA = atoi(strtok(NULL, ","));
        msg.payloadB = atoi(strtok(NULL, ","));
      }
      break;
      case 'B': {
        msg.type = MOVE_BACKWARD_MESSAGE;
        msg.payloadA = atoi(strtok(NULL, ","));
        msg.payloadB = atoi(strtok(NULL, ","));
      }
      break; 
      case 'L': {
        msg.type = TURN_LEFT_MESSAGE;
        msg.payloadA = atoi(strtok(NULL, ","));
        msg.payloadB = atoi(strtok(NULL, ","));
      }
      break; 
      case 'R': {
        msg.type = TURN_RIGHT_MESSAGE;
        msg.payloadA = atoi(strtok(NULL, ","));
        msg.payloadB = atoi(strtok(NULL, ","));
      }
      break;
      case 'S': {
        msg.type = STOP_MESSAGE;
      }
      break;
      case 'f': {
        msg.type = MOVE_FORWARD_BY_MESSAGE;
        msg.payloadA = atoi(strtok(NULL, ","));
      }
      break;
      case 'b': {
        msg.type = MOVE_BACKWARD_BY_MESSAGE;
        msg.payloadA = atoi(strtok(NULL, ","));
      }
      break;
      case 'l': {
        msg.type = TURN_LEFT_BY_MESSAGE;
        msg.payloadA = atoi(strtok(NULL, ","));
      }
      break;
      case 'r': {
        msg.type = TURN_RIGHT_BY_MESSAGE;
        msg.payloadA = atoi(strtok(NULL, ","));
      }
      break;
    }
    return msg;
}

Message createMessageFromController(int x, int y) {
  Message msg;
  int backlash = 15;
  if (abs(512 - y) > abs(512 - x))
  {
    if (y < 512 - backlash) {
      msg.type = TURN_LEFT_MESSAGE;
      int throttle = map(y, 512 - backlash, 0, 0, 255);
      msg.payloadA = throttle;
      msg.payloadB = throttle;
    } 
    else if (y > 512 + backlash) {
      msg.type = TURN_RIGHT_MESSAGE;
      int throttle = map(y, 512 + backlash, 1023, 0, 255);
      msg.payloadA = throttle;
      msg.payloadB = throttle;
    } 
  } else {
      if (x < 512 - backlash) {
      msg.type = MOVE_BACKWARD_MESSAGE;
      int throttle = map(x, 512 - backlash, 0, 0, 255);
      msg.payloadA = throttle;
      msg.payloadB = throttle;
    }
    else if (x > 512 + backlash) {
      msg.type = MOVE_FORWARD_MESSAGE;
      int throttle = map(x, 512 + backlash, 1023, 0, 255);
      msg.payloadA = throttle;
      msg.payloadB = throttle;
    }
  }
  return msg;
}  


