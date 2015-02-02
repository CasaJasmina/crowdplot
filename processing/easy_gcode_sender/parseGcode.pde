int linenumber;
float lastRelativeX;
float lastRelativeY;


String lastCoord[] = new String[10];

ArrayList<String> gc = new ArrayList<String>();

boolean parseGcode() {
  String[] list = split(gCode, "\n");

  String[] activeCell=split(list[0], "_");
  if (!activeCell[0].equals("eof")) {
    activeCellX=Integer.parseInt(activeCell[0]);
    activeCellY=Integer.parseInt(activeCell[1]);

    for (int i=0; i<list.length; i++) {
      gc.add (list[i]);
      println(list[i]);
    }
    println("You have a new GCode to work on!");

    return true;
  } else {
    println("I'm done!");
    return false;
  }
}


void executeGCode() {
  gCodeExecutionStarted=true;

  int tx=(activeCellX*100)-PlotterWidth/2;
  int ty=((nCellY-activeCellY)*100)-PlotterHeight/2;

  plotter.send("G00 X"+tx+" Y"+ty);
  delay(100);
  plotter.send("G92 X0 Y0");
  delay(100);

  while (gc.size ()>1) {
    if (readyToAct) {
      gc.remove(0);

      println(gc.get(0));
      plotter.send(gc.get(0));

      lastCoord= split(gc.get(0), " ");

      println(lastCoord);
      try {
        lastRelativeX=Float.parseFloat(lastCoord[1].substring(1));
        lastRelativeY=Float.parseFloat(lastCoord[2].substring(1));

        println(lastRelativeX);
        println(lastRelativeY);
      }
      catch(Exception e) {
        println(e);
      }
      readyToAct=false;
      println(gc.get(0));
      gc.remove(0);
    }
  }
  //CheckForNewGcode.setIntervalMs(5000);
  float ritrasloX=-tx-lastRelativeX;
  float ritrasloY=-ty-lastRelativeY;

  plotter.send("G00 Z90");
  plotter.send("G00 X"+ritrasloX+" Y"+ritrasloY);
  delay(100);
  plotter.send("G92 X0 Y0");
  delay(100);

  GCodeInit();

  gCodeExecutionStarted=false;

  newDownload();
}

