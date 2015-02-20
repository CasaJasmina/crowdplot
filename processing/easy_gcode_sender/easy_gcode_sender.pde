import org.multiply.processing.*;

import processing.serial.*;
import http.requests.*;


//private TimedEventGenerator CheckForNewGcode;
boolean GO=true;


int bgcolor;           // Background color
int fgcolor;           // Fill color
Serial mySerial;                       // The serial port
int[] serialInArray = new int[3];    // Where we'll put what we receive



int pW;
int pH;

int penUp=90;
int penDown=80;

int motorFeedFast=300;
int motorFeedSlow=100;

int activeCellX=-1;
int activeCellY=-1;

boolean firstContact = false;        // Whether we've heard from the microcontroller

boolean readyToAct=false;
boolean gCodeExecutionStarted=false;

int speed=0;
boolean pen=false;

GCodeSender plotter;

int PlotterWidth = 1200;
int PlotterHeight = 2400;

int CellDim=100;
int nCellX=13;
int nCellY=19;

int monitorScaleFactor=4;
void setup() {

  background(20, 20, 20);
  size(PlotterWidth/monitorScaleFactor+200, PlotterHeight/monitorScaleFactor);  // Stage size
  noStroke(); 
  printArray(Serial.list());
  try {
    String portName = Serial.list()[7];
    mySerial = new Serial(this, portName, 57600);
    while (mySerial.available () > 0) {
    }
  } 
  catch(Exception e) {
    println(e);
  }

  cp5 = new ControlP5(this);

  Group g1 = cp5.addGroup("MOVE")
    .setPosition(PlotterWidth/monitorScaleFactor+50, 20)
      .setBackgroundHeight(100)
        .setBackgroundColor(color(255, 50))
          ;

  cp5.addBang("up")
    .setPosition(10, 30)
      .setSize(20, 20)
        .setGroup(g1)
          ;

  cp5.addBang("down")
    .setPosition(30, 10)
      .setSize(20, 20)
        .setGroup(g1)
          ;
  cp5.addBang("left")
    .setPosition(50, 30)
      .setSize(20, 20)
        .setGroup(g1)
          ;
  cp5.addBang("right")
    .setPosition(30, 50)
      .setSize(20, 20)
        .setGroup(g1)
          ;

  plotter=new GCodeSender(mySerial);
  GCodeInit();
}

void draw() {
  background(20, 20, 20);

  refreshGrid();
}



void serialEvent(Serial p) {

  String inString = p.readString(); 
  print(inString);

  if (inString.charAt(0)=='>') {
    readyToAct=true;
  }
}

void refreshGrid() {
  for (int i=0; i<nCellX; i++) {
    for (int j=0; j<nCellY; j++) {
      stroke(0);
      fill (255);

      if (activeCellX==i&&activeCellY==j) {
        fill(200);
      }

      rect(CellDim/monitorScaleFactor*i, CellDim/monitorScaleFactor*j, CellDim/monitorScaleFactor, CellDim/monitorScaleFactor);
    }
  }
}

