import java.util.Map;

int MAX_POSSIBLE_STEP_OFFSET = 1024;

class Locator {
  private GSketchPad sketchPad;
  private HashMap<Integer, Integer> data = new HashMap<Integer, Integer>();
  private int maxDataValue = 150;
  private PGraphics g;
  private float width, height;
  private int maxDistance = 100;
  private float angleDelta = 15;
  private int lastAngleAdded = 0;
  private float scale;
  private float pointX, pointY;
  
  public Locator(GSketchPad sketchPad) {
    this.sketchPad = sketchPad;
    this.width = sketchPad.getWidth();
    this.height = sketchPad.getHeight();
    g = createGraphics((int)this.width, (int)this.height);
    pointX = width/2;
    pointY = height * 0.97;
    sketchPad.setGraphic(g);
    calculateScale();
    draw();
  }
  
  public void draw() {
    g.beginDraw();
    g.background(0);
    for (Map.Entry me : data.entrySet()) {
      int angle = (int)me.getKey();
      int distance = (int)me.getValue();
      if (angle == lastAngleAdded) drawLine(angle, distance, #a0d000);
      else drawLine(angle, distance, #409000);
    }
    drawGrid();
    g.endDraw();
  }
  
  void drawGrid() {
    g.pushMatrix();
    g.translate(pointX, pointY);
    g.stroke(100,200,0);
    g.textSize(12);
    
    
    // Draw lines
    g.pushMatrix();
    int angle = 45;
    for (int i = 0; i < 360/angle; i++) {
      g.rotate(radians(angle));
      g.line(0,0,0,-max(width, height));
    }
    g.popMatrix();
    
    // Draw circles
    int freq = 25;
    float i = 0;
    while(i*scale <= max(width, height)) {
      i += freq;
      g.noFill();
      g.arc(0,0,i*2*scale,i*2*scale,-(float)Math.PI*2,0);
      g.fill(100,255,0);
      g.text(Integer.toString((int)i)+" cm",5,-i*scale-5);
    }
    g.stroke(200);
    
    g.popMatrix();
  }
  
  void calculateScale() {
    scale = max(width, height) / 2 / maxDistance;
  }
  
  void drawLine(int angle, int distance, color c) {
    g.pushMatrix();
    g.fill(c);
    g.translate(pointX, pointY);
    g.rotate(radians(-angle));
    g.noStroke();
    float delta = sin(radians(angleDelta)) * distance*scale / 2;
    g.triangle(0,0,distance*scale,-delta,distance*scale,delta);
    g.line(0,0,distance * scale,0);
    g.popMatrix();
  }
  
  public void addData(int angle, int distance) {
    println(distance);
    if (distance > maxDistance) {
      maxDistance = distance;
      calculateScale();  
    }
    data.put(angle, distance);
    lastAngleAdded = angle;
    draw();
  }
  
}