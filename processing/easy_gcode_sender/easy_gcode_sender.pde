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

  size(PlotterWidth/8+200, PlotterHeight/8);  // Stage size
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
  
  plotter=new GCodeSender(mySerial);
  GCodeInit();

  //CheckForNewGcode= new TimedEventGenerator(this, "newDownload");
  //CheckForNewGcode.setIntervalMs(5000);
}

void draw() {
  background(255);

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
  background(255);
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

