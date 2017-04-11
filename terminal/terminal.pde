import g4p_controls.*;

import processing.serial.*;

Serial port;
Locator locator;

IntList pressedKeys = new IntList();

void setup() {
  size(1000,600);
  port = new Serial(this, "COM7", 115200); 
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
      int sum = 0;
      try{
       sum = Integer.parseInt(m[1].trim()) + Integer.parseInt(m[2].trim());
      } catch(Exception error) {
        println(error);
      }
      println("L:"+m[1]+" R:"+m[2]+" SUM:"+sum);
    break;
    case "l":
       int position = Integer.parseInt(m[1].trim());
       int distance = Integer.parseInt(m[2].trim());
       locator.addData(position, distance);
       break;
    default:
      println("Unknown message: "+message);
    break;
  }
}

void draw(){
  background(color(0xFFFFFF));
  locator.draw();
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
  int lThrottle = lThrottleSlider.getValueI(), rThrottle = rThrottleSlider.getValueI();
  
  switch(pressedKeyCode) {
    case 87: // W key
      port.write("F,"+lThrottle+","+rThrottle+"\n");
    break;
    case 65: // A key
      port.write("L,"+lThrottle+","+rThrottle+"\n");
    break;
    case 83: // S key 
      port.write("B,"+lThrottle+","+rThrottle+"\n");
    break;
    case 68: // D key
      port.write("R,"+lThrottle+","+rThrottle+"\n");
    break;
  }
}

void sendTurnLeftByMessage(int path) {
  port.write("l,"+path+"\n");
}

void sendTurnRightByMessage(int path) {
  port.write("r,"+path+"\n");
}

void sendMoveForwardByMessage(int path) {
  port.write("f,"+path+"\n");
}

void sendMoveBakwardByMessage(int path) {
  port.write("b,"+path+"\n");
}


void sendStopMessage() {
  port.write("S\n");
}

void throttle_slider_change() {
  println("THROTTLE");
}