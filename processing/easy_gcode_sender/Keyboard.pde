

void keyPressed() {
  if (readyToAct) {

    if (key=='g') {
      GO=!GO;
      //  CheckForNewGcode= new TimedEventGenerator(this, "newDownload");

    }

    if (key=='p') {
      pen=!pen;
      plotter.send("G90");

      if (pen==true) {
        plotter.send("G00" + " " + "Z" + penDown);
      } else {
        plotter.send("G00" + " " + "Z" + penUp);
      }
    }

    if (key=='h') {
      println("going home");

      plotter.send("G90");
      plotter.send("G00" + " " + "Z" + penUp);
      plotter.send("G00 X0 Y0");
      readyToAct=false;
    }

    if (key=='q') {
      println("avvolgiSinistra");

      plotter.send("D00 L-100");

      plotter.send("M114");
      readyToAct=false;
    }
    if (key=='a') {
      println("svolgiSinistra");

      plotter.send("D00 L100");


      plotter.send("M114");

      readyToAct=false;
    }

    if (key=='w') {
      println("avvolgiDestra");

      plotter.send("D00 R-100");


      plotter.send("M114");

      readyToAct=false;
    }
    if (key=='s') {
      println("svolgiDestra");

      plotter.send("D00 R100");


      plotter.send("M114");

      readyToAct=false;
    }

    if (key=='r') {
      println("reset");
      GCodeInit();
    }

    if (key=='d') {
      println("download New File");
      downloadNewFile();
      parseGcode();
      executeGCode();
    }

    if (key == CODED) {
      switch (keyCode) {
      case DOWN:
        println("going down");
        plotter.send("G91");

        plotter.send("G01 F"+ motorFeedSlow + " X0 Y-10");

        readyToAct=false;
        break;
      case UP:
        println("going up");
        plotter.send("G91");

        plotter.send("G01 F"+ motorFeedSlow + " X0 Y10");

        readyToAct=false;
        break;
      case LEFT:
        println("going left");
        plotter.send("G91");

        plotter.send("G01 F"+ motorFeedSlow + " X-10 Y0");

        readyToAct=false;
        break;
      case RIGHT:
        println("going right");
        plotter.send("G91");

        plotter.send("G01 F"+ motorFeedSlow + " X10 Y0");

        readyToAct=false;
        break;
      }
    }
  }
}

