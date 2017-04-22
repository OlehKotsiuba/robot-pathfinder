#include <SPI.h>
#include "RF24.h"
#include "Message.h"
#include <Psx.h> 

Psx Psx;

#define X_AXIS_PIN A0
#define Y_AXIS_PIN A1

#define dataPin 5
#define cmndPin 2
#define attPin 3
#define clockPin 4

RF24 radio(7,8);

byte addresses[][6] = {"1Node", "2Node"};

void setup() {
  Serial.begin(115200);
  
  radio.begin();

  radio.setPayloadSize(5);

  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1,addresses[1]);

  radio.startListening();

  Psx.setupPins(dataPin, cmndPin, attPin, clockPin, 10);
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
    radio.startListening();
    return;
  }

  unsigned int buttonState = Psx.read();

  Serial.println(buttonState & 2);

  Message outRadioMessage = getMessageFromController(buttonState);
  radio.stopListening();
  radio.write( &outRadioMessage, sizeof(Message));
  radio.startListening();
}

Message getMessageFromSerial(char * serialInMsg) {
   Message msg;
   char *msgType = strtok(serialInMsg, ",");
    switch(msgType[0]) {
      case 'F': {
        msg.type = MOVE_FORWARD_MESSAGE;
        //msg.payloadA = atoi(strtok(NULL, ","));
        //msg.payloadB = atoi(strtok(NULL, ","));
      }
      break;
      case 'B': {
        msg.type = MOVE_BACKWARD_MESSAGE;
        //msg.payloadA = atoi(strtok(NULL, ","));
        //msg.payloadB = atoi(strtok(NULL, ","));
      }
      break; 
      case 'L': {
        msg.type = TURN_LEFT_MESSAGE;
        //msg.payloadA = atoi(strtok(NULL, ","));
        //msg.payloadB = atoi(strtok(NULL, ","));
      }
      break; 
      case 'R': {
        msg.type = TURN_RIGHT_MESSAGE;
        //msg.payloadA = atoi(strtok(NULL, ","));
        //msg.payloadB = atoi(strtok(NULL, ","));
      }
      break;
      case 'S': {
        msg.type = STOP_MESSAGE;
      }
      break;
    }
    return msg;
}

Message getMessageFromController(unsigned int state) {
  Message msg;
  int x = analogRead(A4) - 512;
  int y = analogRead(A5) - 512;
  if(state & 1) {
    msg.type = TURN_LEFT_MESSAGE;
  } else if(state& 2) {
    msg.type = MOVE_BACKWARD_MESSAGE;
  } else if (state & 4) {
    msg.type = TURN_RIGHT_MESSAGE;
  } else if (state & 8) {
    msg.type = MOVE_FORWARD_MESSAGE;
  } else if (abs(x) > 100 || abs(y) > 100 ) {
    if (abs(x) > abs(y)) {
      msg.type = x > 0 ? MOVE_FORWARD_MESSAGE : MOVE_BACKWARD_MESSAGE;
    } else {
      msg.type = y > 0 ? TURN_LEFT_MESSAGE : TURN_RIGHT_MESSAGE;
    }
  } else {
    msg.type = STOP_MESSAGE; 
  }
  return msg;
}

