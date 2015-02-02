void GCodeInit() {

 plotter.send("G90");
  // basic configuration =>> G21 (millimiters) G90 (absolute mode) G64 (constant velocity mode) G40 (turn off radius compensation)
  plotter.send("M114");
  plotter.send("M101 T1780 B-1780 L-1040 R1040 I-1 J1");
  plotter.send("D1 L2.6 R2.6");
  plotter.send("G21" + " " + "G90" + " " + "G64" + " " + "G40");
  

  // output.println("( T0 : 0.8 )");
  // T0 => tool select
  // M6 ==> tool change
  // output.println("T0 M6");
  // G17 ==> select the XY plane
  //plotter.send("G17");
  

  // M3 ==> start spindle clockwise
  // S1000 ==> spindle speed
  // output.println("M3 S1000");
  // F... set stepper motors speedd
  // G0 X0.0 Y0.0 => send plotter head to 'home' position 
  // G0 is movement with penup while G1 is movement with pen down -> not so sure about this! #ancheno
  // G0 Z... ==> pen UP
  plotter.send("G00" + " " + "Z" + penUp);
  
  plotter.send("G92 X0 Y0");
  
  plotter.send("G00" + " " + "F" + motorFeedFast + " " + "X0.0" + " " +  "Y0.0");
  



  // disegna i due assi X,Y
  /*
  plotter.send("G0" + " " + "Z" + penDown);
   
   plotter.send("G0 X-205 Y0");
   
   plotter.send("G0 X205 Y0");
   
   plotter.send("G0" + " " + "Z" + penUp);
   
   plotter.send("G0 X0 Y140");
   
   plotter.send("G0" + " " + "Z" + penDown);
   
   plotter.send("G0 X0 Y-140");
   
   plotter.send("G0" + " " + "Z" + penUp);
   
   plotter.send(" ");
   
   */
}

