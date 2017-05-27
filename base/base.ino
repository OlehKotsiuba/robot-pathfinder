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

   if(radio.available()) {
      Message inRadioMessage;   
      radio.read(&inRadioMessage,sizeof(Message));
      switch (inRadioMessage.type) {
        case Message::ENCODER_DATA: 
          Serial.print("E,");
          Serial.print(inRadioMessage.payload.words.l);
          Serial.print(",");
          Serial.println(inRadioMessage.payload.words.h); 
        break;
        case Message::LOCATOR_DATA:
          Serial.print("l,");
          Serial.print(inRadioMessage.payload.words.l);
          Serial.print(",");
          Serial.println(inRadioMessage.payload.words.h); 
        break;
      }
   }



  unsigned int buttonState = Psx.read();

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
        msg.type = Message::MOVE_FORWARD;
        //msg.payloadA = atoi(strtok(NULL, ","));
        //msg.payloadB = atoi(strtok(NULL, ","));
      }
      break;
      case 'B': {
        msg.type = Message::MOVE_BACKWARD;
        //msg.payloadA = atoi(strtok(NULL, ","));
        //msg.payloadB = atoi(strtok(NULL, ","));
      }
      break; 
      case 'L': {
        msg.type = Message::TURN_LEFT;
        //msg.payloadA = atoi(strtok(NULL, ","));
        //msg.payloadB = atoi(strtok(NULL, ","));
      }
      break; 
      case 'R': {
        msg.type = Message::TURN_RIGHT;
        //msg.payloadA = atoi(strtok(NULL, ","));
        //msg.payloadB = atoi(strtok(NULL, ","));
      }
      break;
      case 'S': {
        msg.type = Message::STOP;
      }
      break;
    }
    return msg;
}

Message getMessageFromController(unsigned int state) {
  Message msg;
  long x = analogRead(A4) - 512;
  long y = analogRead(A5) - 512;
  int hX = analogRead(A6) - 512;
  int hY = analogRead(A7) - 512; 
  if(state & psxLeft) {
    msg.type = Message::TURN_LEFT;
  } else if(state& psxDown) {
    msg.type = Message::MOVE_BACKWARD;
  } else if (state & psxRight) {
    msg.type = Message::TURN_RIGHT;
  } else if (state & psxUp) {
    msg.type = Message::MOVE_FORWARD;
  } else if (abs(x) > 25 || abs(y) > 25 ) {
    msg.type = Message::MOVE_ANALOG;
    long length = constrain(sqrt(x*x + y*y) / 2, 0, 255);
    if (x < 0) length = -length;
    msg.payload.words.l = length;
    msg.payload.words.h = length;
    if (y > 0) {
      msg.payload.words.l = (long)msg.payload.words.l * (512 - y) / 512; 
    } else if (y < 0) {
      msg.payload.words.h = (long)msg.payload.words.h * (512 + y) / 512;
    }  
  } else if (abs(hX) > 25 || abs(hY) > 25) {
     msg.type = Message::SET_HEADING;
     msg.payload.words.l = atan2(hX, hY) / PI * 180;
  }
  else if (state & psxSqu) {
    msg.type = Message::SET_MODE;
    msg.payload.dWord = 0;
  } else if (state & psxTri) {
    msg.type = Message::SET_MODE;
    msg.payload.dWord = 1;
  } else {
    msg.type = Message::Type::STOP; 
  }
  return msg;
}

