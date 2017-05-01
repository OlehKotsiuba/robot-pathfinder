import g4p_controls.*;

import processing.serial.*;

Serial port;
Locator locator;

IntList pressedKeys = new IntList();

void setup() {
  size(450,300);
  port = new Serial(this, "COM3", 115200); 
  port.bufferUntil('\n'); 
  createGUI();
  locator = new Locator(locatorSketchPad);
}

void serialEvent(Serial p) {
  String message = p.readString();
  message = message.replace('\n',',');
  String[] m = message.split(",");
  switch(m[0]) {
    case "E":
      float sum = 0;
      try{
       sum = Float.parseFloat(m[1].trim()) / Float.parseFloat(m[2].trim());
      } catch(Exception error) {
        println(error);
      }
      println("tics:"+m[1]+" distance:"+m[2]+" RES:"+sum);
    break;
    case "l":
     int position = Integer.parseInt(m[1].trim());
     int distance = Integer.parseInt(m[2].trim());
     locator.addData(position, distance);
   break;
    case "p":
      int error = Integer.parseInt(m[1].trim());
      println("Error: ", error);
      break;
    default:
      println("Unknown message: "+message);
    break;
  }
}

void draw(){
  background(color(0xFFFFFF));
}
 
void keyPressed() {
  if(!pressedKeys.hasValue(keyCode)) {
    pressedKeys.push(keyCode);
  }
  sendMoveMessage(keyCode);
}

void keyReleased()
{
  pressedKeys.removeValue(keyCode);
  if (pressedKeys.size() > 0) {
    int lastPressedButton = pressedKeys.get(pressedKeys.size()-1);
    sendMoveMessage(lastPressedButton);
  } else {
    sendStopMessage();
  }
} 

void sendMoveMessage(int pressedKeyCode) {
  switch(pressedKeyCode) {
    case 87: // W key
      port.write("F\n");
    break;
    case 65: // A key
      port.write("L\n");
    break;
    case 83: // S key 
      port.write("B\n");
    break;
    case 68: // D key
      port.write("R\n");
    break;
  }
}


void sendStopMessage() {
  port.write("S\n");
}