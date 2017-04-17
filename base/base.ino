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
  radio.setPayloadSize(5);

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
    byte retry = 0;
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

