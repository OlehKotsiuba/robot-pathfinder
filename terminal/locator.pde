int MAX_POSSIBLE_STEP_OFFSET = 1024;

class Locator {
  private GSketchPad sketchPad;
  private int[] data = new int[maxStepOffset * 2 / scanStepWidth + 1];
  private int maxDataValue = 150;
  private PGraphics g;
  private int lastIndexAdded;
  
  public Locator(GSketchPad sketchPad) {
    this.sketchPad = sketchPad;
    g = createGraphics(int(sketchPad.getWidth()), int(sketchPad.getHeight()));
    sketchPad.setGraphic(g);
    println(maxAngle);
  }
  
  public void setParams(int maxStepOffset, int scanStepWidth) {
    this.maxStepOffset = maxStepOffset;
    this.scanStepWidth = scanStepWidth;
    this.scanAngle = (float)(scanStepWidth / MAX_POSSIBLE_STEP_OFFSET * Math.PI);
    this.maxAngle = (float)(maxStepOffset / MAX_POSSIBLE_STEP_OFFSET * Math.PI);
    data = new int[maxStepOffset * 2 / scanStepWidth + 1];
  }
  
  public void draw() {
    g.beginDraw();
    g.background(0);
    for(int i = 0; i < data.length; i++) {
      float x = float(i) / data.length * sketchPad.getWidth();
      g.stroke(255,0,0);
      if (i != lastIndexAdded) g.fill(255,0,0);
      else g.fill(0,255,0);
      g.arc(sketchPad.getWidth()/2,
      sketchPad.getHeight()/2,
      float(data[i]) / maxDataValue * sketchPad.getWidth(),
      float(data[i]) / maxDataValue * sketchPad.getWidth(),
      (float(i) - data.length/2) * scanAngle - scanAngle/2 - (float)Math.PI/2,
      (float(i) - data.length/2) * scanAngle + scanAngle/2 - (float)Math.PI/2,
      PIE);
    }
    g.endDraw();
  }
  
  public void addData(int position, int distance) {
    lastIndexAdded = (position + maxStepOffset) / scanStepWidth;
    data[lastIndexAdded] = distance;
  }
  
}