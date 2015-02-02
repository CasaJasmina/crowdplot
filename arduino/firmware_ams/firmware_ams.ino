
// Draw robot
// dan@marginallycelver.com 2012 feb 11
//------------------------------------------------------------------------------
// Copyright at end of file.  Please see
// http://www.github.com/MarginallyClever/Makelangelo for more information.



//------------------------------------------------------------------------------
// CONSTANTS
//------------------------------------------------------------------------------
//#define MOTHERBOARD 1  // Adafruit Motor Shield 1
#define MOTHERBOARD 2  // Adafruit Motor Shield 2

// Increase this number to see more output
#define VERBOSE         (0)

// Comment out this line to disable SD cards.
//#define USE_SD_CARD       (1)
// Comment out this line to disable findHome and limit switches
//#define USE_LIMIT_SWITCH  (1)



// which motor is on which pin?
#define M1_PIN          (2)
#define M2_PIN          (1)

// which limit switch is on which pin?
#define L_PIN           (A3)
#define R_PIN           (A5)

// NEMA17 are 200 steps (1.8 degrees) per turn.  If a spool is 0.8 diameter
// then it is 2.5132741228718345 circumference, and
// 2.5132741228718345 / 200 = 0.0125663706 thread moved each step.
// NEMA17 are rated up to 3000RPM.  Adafruit can handle >1000RPM.
// These numbers directly affect the maximum velocity.
#define STEPS_PER_TURN  (48)


#define NUM_TOOLS  (6)


// *****************************************************************************
// *** Don't change the constants below unless you know what you're doing.   ***
// *****************************************************************************

// switch sensitivity
#define SWITCH_HALF     (512)

// servo angles for pen control
#define PEN_UP_ANGLE    (90)
#define PEN_DOWN_ANGLE  (70)  // Some servos don't like 0 degrees
#define PEN_DELAY       (250)  // in ms

#define MAX_STEPS_S     (STEPS_PER_TURN*MAX_RPM/60.0)  // steps/s

#define MAX_FEEDRATE    (3500)
#define MIN_FEEDRATE    (200) // steps / second

// for arc directions
#define ARC_CW          (1)
#define ARC_CCW         (-1)
// Arcs are split into many line segments.  How long are the segments?
#define CM_PER_SEGMENT   (0.2)

// Serial communication bitrate
#define BAUD            (57600)
// Maximum length of serial input message.
#define MAX_BUF         (64)

// servo pin differs based on device
#define SERVO_PIN       (10)

#define TIMEOUT_OK      (1000)  // 1/4 with no instruction?  Make sure PC knows we are waiting.

#ifndef USE_SD_CARD
#define File int
#endif


#define M1_STEP  m1->step
#define M2_STEP  m2->step
#define M1_ONESTEP(x)  m1->onestep(x, SINGLE);
#define M2_ONESTEP(x)  m2->onestep(x, SINGLE);
// stacked motor shields have different addresses. The default is 0x60
#define SHIELD_ADDRESS (0x60)

//------------------------------------------------------------------------------
// EEPROM MEMORY MAP
//------------------------------------------------------------------------------
#define EEPROM_VERSION   4             // Increment EEPROM_VERSION when adding new variables
#define ADDR_VERSION     0             // address of the version number (one byte)
#define ADDR_UUID        1             // address of the UUID (long - 4 bytes)
#define ADDR_SPOOL_DIA1  5             // address of the spool diameter (float - 4 bytes)
#define ADDR_SPOOL_DIA2  9             // address of the spool diameter (float - 4 bytes)


//------------------------------------------------------------------------------
// INCLUDES
//------------------------------------------------------------------------------

#include <Wire.h>
#include <Adafruit_MotorShield.h>

// Default servo library
#include <Servo.h>



// Saving config
#include <EEPROM.h>
#include <Arduino.h>  // for type definitions

#include "Vector3.h"


//------------------------------------------------------------------------------
// VARIABLES
//------------------------------------------------------------------------------

#if MOTHERBOARD == 2
// Initialize Adafruit stepper controller
Adafruit_MotorShield AFMS0 = Adafruit_MotorShield(SHIELD_ADDRESS);
Adafruit_StepperMotor *m1;
Adafruit_StepperMotor *m2;
#endif

static Servo s1;

// robot UID
int robot_uid = 0;

// plotter limits
// all distances are relative to the calibration point of the plotter.
// (normally this is the center of the drawing area)
static float limit_top = 0;  // distance to top of drawing area.
static float limit_bottom = 0;  // Distance to bottom of drawing area.
static float limit_right = 0;  // Distance to right of drawing area.
static float limit_left = 0;  // Distance to left of drawing area.

// what are the motors called?
char m1d = 'L';
char m2d = 'R';

// which way are the spools wound, relative to motor movement?
int M1_REEL_IN  = FORWARD;
int M1_REEL_OUT = BACKWARD;
int M2_REEL_IN  = FORWARD;
int M2_REEL_OUT = BACKWARD;

// calculate some numbers to help us find feed_rate
float SPOOL_DIAMETER1 = 2.6;
float THREADPERSTEP1;  // thread per step

float SPOOL_DIAMETER2 = 2.6;
float THREADPERSTEP2;  // thread per step

// plotter position.
static float posx, velx;
static float posy, vely;
static float posz;  // pen state
static float feed_rate = 0;
static long step_delay;


// motor position
static long laststep1, laststep2;

static char absolute_mode = 1; // absolute or incremental programming mode?
static float mode_scale;   // mm or inches?
static char mode_name[3];


// Serial comm reception
static char buffer[MAX_BUF + 1]; // Serial buffer
static int sofar;               // Serial buffer progress
static long last_cmd_time;      // prevent timeouts

Vector3 tool_offset[NUM_TOOLS];
int current_tool = 0;

long line_number;


int speed = 200;




//THOUSE Are MY CONSTANTS

float minDiam1=24.7;
float minDiam2=24.7;




//------------------------------------------------------------------------------
// METHODS
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// calculate max velocity, threadperstep.
static void adjustSpoolDiameter(float diameter1, float diameter2) {
  SPOOL_DIAMETER1 = diameter1;
  float SPOOL_CIRC = SPOOL_DIAMETER1 * PI; // circumference
  THREADPERSTEP1 = SPOOL_CIRC / STEPS_PER_TURN / 10; // thread per step

  SPOOL_DIAMETER2 = diameter2;
  SPOOL_CIRC = SPOOL_DIAMETER2 * PI; // circumference
  THREADPERSTEP2 = SPOOL_CIRC / STEPS_PER_TURN / 10; // thread per step

#if VERBOSE > 2
  Serial.print(F("SpoolDiameter1 = "));  Serial.println(SPOOL_DIAMETER1, 3);
  Serial.print(F("SpoolDiameter2 = "));  Serial.println(SPOOL_DIAMETER2, 3);
  Serial.print(F("THREADPERSTEP1="));  Serial.println(THREADPERSTEP1, 3);
  Serial.print(F("THREADPERSTEP2="));  Serial.println(THREADPERSTEP2, 3);
  Serial.print(F("MAX_VEL="));  Serial.println(MAX_VEL, 3);
#endif
}


//------------------------------------------------------------------------------
// returns angle of dy/dx as a value from 0...2PI
static float atan3(float dy, float dx) {
  float a = atan2(dy, dx);
  if (a < 0) a = (PI * 2.0) + a;
  return a;
}


//------------------------------------------------------------------------------
static char readSwitches() {
#ifdef USE_LIMIT_SWITCH
  // get the current switch state
  return ( (analogRead(L_PIN) < SWITCH_HALF) | (analogRead(R_PIN) < SWITCH_HALF) );
#else
  return 0;
#endif  // USE_LIMIT_SWITCH
}


//------------------------------------------------------------------------------
// feed rate is given in units/min and converted to cm/s
static void setFeedRate(float v) {
  if (feed_rate == v) return;
  feed_rate = v;

  if (v > MAX_FEEDRATE) v = MAX_FEEDRATE;
  if (v < MIN_FEEDRATE) v = MIN_FEEDRATE;

  step_delay = 1000000.0 / v;

#if VERBOSE > 1
  Serial.print(F("feed_rate="));  Serial.println(feed_rate);
  Serial.print(F("step_delay="));  Serial.println(step_delay);
#endif
}


//------------------------------------------------------------------------------
static void printFeedRate() {
  Serial.print(F("F"));
  Serial.println(feed_rate);
}


//------------------------------------------------------------------------------
// Change pen state.
static void setPenAngle(int pen_angle) {
  if (posz != pen_angle) {


    if (pen_angle > posz) {
      while (pen_angle > posz) {
        posz++;
        s1.write(posz);
        delay(100);
      }
    } else if (pen_angle < posz) {
      while (pen_angle < posz) {
        posz--;
        s1.write(posz);
        delay(100);

      }

    }
    delay(500);
  }
}


//------------------------------------------------------------------------------
// Inverse Kinematics - turns XY coordinates into lengths L1,L2
static void IK(float x, float y, long &l1, long &l2) {
  // find length to M1
  float dy = y - limit_top;
  float dx = x - limit_left;
  l1 = floor( sqrt(dx * dx + dy * dy) / THREADPERSTEP1 );
  // find length to M2
  dx = limit_right - x;
  l2 = floor( sqrt(dx * dx + dy * dy) / THREADPERSTEP2 );
  
  
  /*
  //this is trying to compensate for the wire that goes arount the pulley
  float newDiam1=minDiam1+((20625-l1)*0.00018);
  float newDiam2=minDiam2+((20625-l2)*0.00018);
 
  
  adjustSpoolDiameter(newDiam1, newDiam2);
  SaveSpoolDiameter();
  
  */

}


//------------------------------------------------------------------------------
// Forward Kinematics - turns L1,L2 lengths into XY coordinates
// use law of cosines: theta = acos((a*a+b*b-c*c)/(2*a*b));
// to find angle between M1M2 and M1P where P is the plotter position.
static void FK(float l1, float l2, float &x, float &y) {
  float a = l1 * THREADPERSTEP1;
  float b = (limit_right - limit_left);
  float c = l2 * THREADPERSTEP2;

  // slow, uses trig
  //float theta = acos((a*a+b*b-c*c)/(2.0*a*b));
  //x = cos(theta)*l1 + limit_left;
  //y = sin(theta)*l1 + limit_top;
  // but we know that cos(acos(i)) = i
  // and we know that sin(acos(i)) = sqrt(1-i*i)
  float i = (a * a + b * b - c * c) / (2.0 * a * b);
  x = i * l1 + limit_left;
  y = sqrt(1.0 - i * i) * l1 + limit_top;
}


//------------------------------------------------------------------------------
void pause(long ms) {
  delay(ms / 1000);
  delayMicroseconds(ms % 1000);
}


//------------------------------------------------------------------------------
static void line(float x, float y, float z) {
  long l1, l2;
  IK(x, y, l1, l2);
  long d1 = l1 - laststep1;
  long d2 = l2 - laststep2;

  long ad1 = abs(d1);
  long ad2 = abs(d2);
  int dir1 = d1 < 0 ? M1_REEL_IN : M1_REEL_OUT;
  int dir2 = d2 < 0 ? M2_REEL_IN : M2_REEL_OUT;
  long over = 0;
  long i;

  setPenAngle((int)z);
  // bresenham's line algorithm.
  if (ad1 > ad2) {
    for (i = 0; i < ad1; ++i) {
      M1_ONESTEP(dir1);
      over += ad2;
      if (over >= ad1) {
        over -= ad1;
        M2_ONESTEP(dir2);
      }
      delayMicroseconds(step_delay);
      if (readSwitches()) return;
    }
  } else {
    for (i = 0; i < ad2; ++i) {
      M2_ONESTEP(dir2);
      over += ad1;
      if (over >= ad2) {
        over -= ad2;
        M1_ONESTEP(dir1);
      }
      delayMicroseconds(step_delay);
      if (readSwitches()) return;
    }
  }

  laststep1 = l1;
  laststep2 = l2;
  posx = x;
  posy = y;
  posz = z;
}


//------------------------------------------------------------------------------
static void line_safe(float x, float y, float z) {
  // split up long lines to make them straighter?
  float dx = x - posx;
  float dy = y - posy;

  float len = sqrt(dx * dx + dy * dy);

  if (len <= CM_PER_SEGMENT) {
    line(x, y, z);
    return;
  }

  // too long!
  long pieces = floor(len / CM_PER_SEGMENT);
  float x0 = posx;
  float y0 = posy;
  float z0 = posz;
  float a;
  for (long j = 1; j < pieces; ++j) {
    a = (float)j / (float)pieces;

    line((x - x0)*a + x0,
         (y - y0)*a + y0,
         (z - z0)*a + z0);
  }
  line(x, y, z);
}


//------------------------------------------------------------------------------
// This method assumes the limits have already been checked.
// This method assumes the start and end radius match.
// This method assumes arcs are not >180 degrees (PI radians)
// cx/cy - center of circle
// x/y - end position
// dir - ARC_CW or ARC_CCW to control direction of arc
static void arc(float cx, float cy, float x, float y, float z, float dir) {
  // get radius
  float dx = posx - cx;
  float dy = posy - cy;
  float radius = sqrt(dx * dx + dy * dy);

  // find angle of arc (sweep)
  float angle1 = atan3(dy, dx);
  float angle2 = atan3(y - cy, x - cx);
  float theta = angle2 - angle1;

  if (dir > 0 && theta < 0) angle2 += 2 * PI;
  else if (dir < 0 && theta > 0) angle1 += 2 * PI;

  theta = angle2 - angle1;

  // get length of arc
  // float circ=PI*2.0*radius;
  // float len=theta*circ/(PI*2.0);
  // simplifies to
  float len = abs(theta) * radius;

  int i, segments = floor( len / CM_PER_SEGMENT );

  float nx, ny, nz, angle3, scale;

  for (i = 0; i < segments; ++i) {
    // interpolate around the arc
    scale = ((float)i) / ((float)segments);

    angle3 = ( theta * scale ) + angle1;
    nx = cx + cos(angle3) * radius;
    ny = cy + sin(angle3) * radius;
    nz = ( z - posz ) * scale + posz;
    // send it to the planner
    line(nx, ny, nz);
  }

  line(x, y, z);
}


//------------------------------------------------------------------------------
// instantly move the virtual plotter position
// does not validate if the move is valid
static void teleport(float x, float y) {
  posx = x;
  posy = y;

  // @TODO: posz?
  long L1, L2;
  IK(posx, posy, L1, L2);
  laststep1 = L1;
  laststep2 = L2;
}


//------------------------------------------------------------------------------
static void help() {
  Serial.print(F("\n\nHELLO WORLD! I AM DRAWBOT #"));
  Serial.println(robot_uid);
  Serial.println(F("M100 - display this message"));
  Serial.println(F("M101 [Tx.xx] [Bx.xx] [Rx.xx] [Lx.xx]"));
  Serial.println(F("       - display/update board dimensions."));
  Serial.println(F("As well as the following G-codes (http://en.wikipedia.org/wiki/G-code):"));
  Serial.println(F("G00,G01,G02,G03,G04,G28,G90,G91,G92,M18,M114"));
}

//------------------------------------------------------------------------------
static void where() {
  Serial.print(F("X"));
  Serial.print(posx);
  Serial.print(F(" Y"));
  Serial.print(posy);
  Serial.print(F(" Z"));
  Serial.print(posz);
  printFeedRate();
  Serial.print(F("\n"));
}


//------------------------------------------------------------------------------
static void printConfig() {
  Serial.print(m1d);              Serial.print(F("="));
  Serial.print(limit_top);        Serial.print(F(","));
  Serial.print(limit_left);       Serial.print(F("\n"));
  Serial.print(m2d);              Serial.print(F("="));
  Serial.print(limit_top);        Serial.print(F(","));
  Serial.print(limit_right);      Serial.print(F("\n"));
  Serial.print(F("Bottom="));     Serial.println(limit_bottom);
  where();
}


//------------------------------------------------------------------------------
// from http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1234477290/3
void EEPROM_writeLong(int ee, long value) {
  byte* p = (byte*)(void*)&value;
  for (int i = 0; i < sizeof(value); i++)
    EEPROM.write(ee++, *p++);
}


//------------------------------------------------------------------------------
// from http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1234477290/3
float EEPROM_readLong(int ee) {
  long value = 0;
  byte* p = (byte*)(void*)&value;
  for (int i = 0; i < sizeof(value); i++)
    *p++ = EEPROM.read(ee++);
  return value;
}


//------------------------------------------------------------------------------
static void SaveUID() {
  EEPROM_writeLong(ADDR_UUID, (long)robot_uid);
}

//------------------------------------------------------------------------------
static void SaveSpoolDiameter() {
  EEPROM_writeLong(ADDR_SPOOL_DIA1, SPOOL_DIAMETER1 * 10000);
  EEPROM_writeLong(ADDR_SPOOL_DIA2, SPOOL_DIAMETER2 * 10000);
}


//------------------------------------------------------------------------------
static void LoadConfig() {
  char version_number = EEPROM.read(ADDR_VERSION);
  if (version_number < 3 || version_number > EEPROM_VERSION) {
    // If not the current EEPROM_VERSION or the EEPROM_VERSION is sullied (i.e. unknown data)
    // Update the version number
    EEPROM.write(ADDR_VERSION, EEPROM_VERSION);
    // Update robot uuid
    robot_uid = 0;
    SaveUID();
    // Update spool diameter variables
    SaveSpoolDiameter();
  }
  if (version_number == 3) {
    // Retrieve Stored Configuration
    robot_uid = EEPROM_readLong(ADDR_UUID);
    adjustSpoolDiameter((float)EEPROM_readLong(ADDR_SPOOL_DIA1) / 10000.0f,
                        (float)EEPROM_readLong(ADDR_SPOOL_DIA1) / 10000.0f); //3 decimal places of percision is enough
    // save the new data so the next load doesn't screw up one bobbin size
    SaveSpoolDiameter();
    // update the EEPROM version
    EEPROM.write(ADDR_VERSION, EEPROM_VERSION);
  } else if (version_number == EEPROM_VERSION) {
    // Retrieve Stored Configuration
    robot_uid = EEPROM_readLong(ADDR_UUID);
    adjustSpoolDiameter((float)EEPROM_readLong(ADDR_SPOOL_DIA1) / 10000.0f,
                        (float)EEPROM_readLong(ADDR_SPOOL_DIA2) / 10000.0f); //3 decimal places of percision is enough
  } else {
    // Code should not get here if it does we should display some meaningful error message
    Serial.println(F("An Error Occurred during LoadConfig"));
  }
}




//------------------------------------------------------------------------------



void disable_motors() {
#if MOTHERBOARD == 1
  m1.release();
  m2.release();
#endif
#if MOTHERBOARD == 2
  m1->release();
  m2->release();
#endif
}


void activate_motors() {
  M1_STEP(1, 1);  M1_STEP(1, -1);
  M2_STEP(1, 1);  M2_STEP(1, -1);
}


/**
 * Look for character /code/ in the buffer and read the float that immediately follows it.
 * @return the value found.  If nothing is found, /val/ is returned.
 * @input code the character to look for.
 * @input val the return value if /code/ is not found.
 **/
float parsenumber(char code, float val) {
  char *ptr = buffer;
  while (ptr && *ptr && ptr < buffer + sofar) {
    if (*ptr == code) {
      return atof(ptr + 1);
    }
    ptr = strchr(ptr, ' ') + 1;
  }
  return val;
}




//------------------------------------------------------------------------------
void set_tool_offset(int axis, float x, float y, float z) {
  tool_offset[axis].x = x;
  tool_offset[axis].y = y;
  tool_offset[axis].z = z;
}


//------------------------------------------------------------------------------
Vector3 get_end_plus_offset() {
  return Vector3(tool_offset[current_tool].x + posx,
                 tool_offset[current_tool].y + posy,
                 tool_offset[current_tool].z + posz);
}


//------------------------------------------------------------------------------
void tool_change(int tool_id) {
  if (tool_id < 0) tool_id = 0;
  if (tool_id > NUM_TOOLS) tool_id = NUM_TOOLS;
  current_tool = tool_id;
}


//------------------------------------------------------------------------------
void processConfig() {
  limit_top = parsenumber('T', limit_top);
  limit_bottom = parsenumber('B', limit_bottom);
  limit_right = parsenumber('R', limit_right);
  limit_left = parsenumber('L', limit_left);

  char gg = parsenumber('G', m1d);
  char hh = parsenumber('H', m2d);
  char i = parsenumber('I', 0);
  char j = parsenumber('J', 0);
  if (i != 0) {
    if (i > 0) {
      M1_REEL_IN  = HIGH;
      M1_REEL_OUT = LOW;
    } else {
      M1_REEL_IN  = LOW;
      M1_REEL_OUT = HIGH;
    }
  }
  if (j != 0) {
    if (j > 0) {
      M2_REEL_IN  = HIGH;
      M2_REEL_OUT = LOW;
    } else {
      M2_REEL_IN  = LOW;
      M2_REEL_OUT = HIGH;
    }
  }

  // @TODO: check t>b, r>l ?
  printConfig();

  teleport(0, 0);
}


//------------------------------------------------------------------------------
static void processCommand() {
  // blank lines
  if (buffer[0] == ';') return;

  long cmd;

  // is there a line number?
  cmd = parsenumber('N', -1);
  if (cmd != -1 && buffer[0] == 'N') { // line number must appear first on the line
    if ( cmd != line_number ) {
      // Wrong line number error
      Serial.print(F("BADLINENUM "));
      Serial.println(line_number);
      return;
    }


    // is there a checksum?
    if (strchr(buffer, '*') != 0) {
      // Yes.  Is it valid?
      unsigned char checksum = 0;
      int c = 0;
      while (buffer[c] != '*' && c < MAX_BUF) checksum ^= buffer[c++];
      c++; // skip *
      unsigned char against = (unsigned char)strtod(buffer + c, NULL);
      if ( checksum != against ) {
        Serial.print(F("BADCHECKSUM "));
        Serial.println(line_number);
        return;
      }
    } else {
      Serial.print(F("NOCHECKSUM "));
      Serial.println(line_number);
      return;
    }

    line_number++;
  }

  if (!strncmp(buffer, "UID", 3)) {
    robot_uid = atoi(strchr(buffer, ' ') + 1);
    SaveUID();
  }

  cmd = parsenumber('M', -1);
  switch (cmd) {
    case 18:  disable_motors();  break;
    case 17:  activate_motors();  break;
    case 100:  help();  break;
    case 101:  processConfig();  break;
    case 110:  line_number = parsenumber('N', line_number);  break;
    case 114:  where();  break;
  }

  cmd = parsenumber('G', -1);
  switch (cmd) {
    case 0:
    case 1: {  // line
        Vector3 offset = get_end_plus_offset();
        setFeedRate(parsenumber('F', feed_rate));
        line_safe( parsenumber('X', (absolute_mode ? offset.x : 0) * 10) * 0.1 + (absolute_mode ? 0 : offset.x),
                   parsenumber('Y', (absolute_mode ? offset.y : 0) * 10) * 0.1 + (absolute_mode ? 0 : offset.y),
                   parsenumber('Z', (absolute_mode ? offset.z : 0)) + (absolute_mode ? 0 : offset.z) );
        break;
      }
    case 2:
    case 3: {  // arc
        Vector3 offset = get_end_plus_offset();
        setFeedRate(parsenumber('F', feed_rate));
        arc(parsenumber('I', (absolute_mode ? offset.x : 0) * 10) * 0.1 + (absolute_mode ? 0 : offset.x),
            parsenumber('J', (absolute_mode ? offset.y : 0) * 10) * 0.1 + (absolute_mode ? 0 : offset.y),
            parsenumber('X', (absolute_mode ? offset.x : 0) * 10) * 0.1 + (absolute_mode ? 0 : offset.x),
            parsenumber('Y', (absolute_mode ? offset.y : 0) * 10) * 0.1 + (absolute_mode ? 0 : offset.y),
            parsenumber('Z', (absolute_mode ? offset.z : 0)) + (absolute_mode ? 0 : offset.z),
            (cmd == 2) ? -1 : 1);
        break;
      }
    case 4:  // dwell
      pause(parsenumber('S', 0) + parsenumber('P', 0) * 1000.0f);
      break;
    case 20: // inches -> cm
      mode_scale = 2.54f; // inches -> cm
      strcpy(mode_name, "in");
      printFeedRate();
      break;
    case 21:
      mode_scale = 0.1; // mm -> cm
      strcpy(mode_name, "mm");
      printFeedRate();
      break;
    case 54:
    case 55:
    case 56:
    case 57:
    case 58:
    case 59: {  // 54-59 tool offsets
        int tool_id = cmd - 54;
        set_tool_offset(tool_id, parsenumber('X', tool_offset[tool_id].x),
                        parsenumber('Y', tool_offset[tool_id].y),
                        parsenumber('Z', tool_offset[tool_id].z));
        break;
      }
    case 90:  absolute_mode = 1;  break; // absolute mode
    case 91:  absolute_mode = 0;  break; // relative mode
    case 92: {  // set position (teleport)
        Vector3 offset = get_end_plus_offset();
        teleport( parsenumber('X', offset.x),
                  parsenumber('Y', offset.y)
                  //,
                  //parsenumber('Z',offset.z)
                );
        break;
      }
  }

  cmd = parsenumber('D', -1);
  switch (cmd) {
    case 0: {
        // move one motor
        char *ptr = strchr(buffer, ' ') + 1;
        int amount = atoi(ptr + 1);
        int i, dir;

        if (*ptr == m1d) {
          dir = amount < 0 ? M1_REEL_IN : M1_REEL_OUT;
          amount = abs(amount);
          m1->setSpeed(speed);
          m1->step(amount, dir, SINGLE);

        } else if (*ptr == m2d) {
          dir = amount < 0 ? M2_REEL_IN : M2_REEL_OUT;
          amount = abs(amount);
          m2->setSpeed(speed);

          m2->step(amount, dir, SINGLE);

        }
      }
      break;
    case 1: {
        // adjust spool diameters
        float amountL = parsenumber('L', SPOOL_DIAMETER1);
        float amountR = parsenumber('R', SPOOL_DIAMETER2);

        float tps1 = THREADPERSTEP1;
        float tps2 = THREADPERSTEP2;
        adjustSpoolDiameter(amountL, amountR);
        if (THREADPERSTEP1 != tps1 || THREADPERSTEP2 != tps2) {
          // Update EEPROM
          SaveSpoolDiameter();
        }
      }
      break;
    case 2:
      Serial.print('L');  Serial.print(SPOOL_DIAMETER1);
      Serial.print(F(" R"));   Serial.println(SPOOL_DIAMETER2);
      break;
  }
}


//------------------------------------------------------------------------------
/**
 * prepares the input buffer to receive a new message and tells the serial connected device it is ready for more.
 */
void ready() {
  sofar = 0; // clear input buffer
  Serial.print(F("\n> "));  // signal ready to receive input
  last_cmd_time = millis();
}


//------------------------------------------------------------------------------
void tools_setup() {
  for (int i = 0; i < NUM_TOOLS; ++i) {
    set_tool_offset(i, 0, 0, 0);
  }
}


//------------------------------------------------------------------------------
void setup() {
  LoadConfig();

  // initialize the read buffer
  sofar = 0;
  // start communications
  Serial.begin(BAUD);
  while (!Serial) {

  }
  Serial.print(F("\n\nHELLO WORLD! I AM DRAWBOT #"));
  Serial.println(robot_uid);


#if MOTHERBOARD == 2
  // start the shield
  AFMS0.begin();
  m1 = AFMS0.getStepper(STEPS_PER_TURN, M2_PIN);
  m2 = AFMS0.getStepper(STEPS_PER_TURN, M1_PIN);
#endif

  // initialize the scale
  strcpy(mode_name, "mm");
  mode_scale = 0.1;

  setFeedRate(MAX_FEEDRATE);  // *30 because i also /2

  // servo should be on SER1, pin 10.
  s1.attach(SERVO_PIN);
  s1.write(80);

  // turn on the pull up resistor
  digitalWrite(L_PIN, HIGH);
  digitalWrite(R_PIN, HIGH);

  tools_setup();

  // initialize the plotter position.
  teleport(0, 0);
  velx = 0;
  velx = 0;
  setPenAngle(PEN_UP_ANGLE);

  // display the help at startup.
  help();
  ready();
}


//------------------------------------------------------------------------------
// See: http://www.marginallyclever.com/2011/10/controlling-your-arduino-through-the-serial-monitor/
void Serial_listen() {
  // listen for serial commands
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (sofar < MAX_BUF) buffer[sofar++] = c;
    if (c == '\n' || c == '\r') {
      buffer[sofar] = 0;

#if VERBOSE > 0
      // echo confirmation
      Serial.println(buffer);
#endif

      // do something with the command
      processCommand();
      ready();
      break;
    }
  }
}


//------------------------------------------------------------------------------
void loop() {
  Serial_listen();

  // The PC will wait forever for the ready signal.
  // if Arduino hasn't received a new instruction in a while, send ready() again
  // just in case USB garbled ready and each half is waiting on the other.
  if ( (millis() - last_cmd_time) > TIMEOUT_OK ) {
    ready();
  }
}


/**
 * This file is part of DrawbotGUI.
 *
 * DrawbotGUI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DrawbotGUI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */
