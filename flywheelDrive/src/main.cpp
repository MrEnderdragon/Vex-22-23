/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       henryzhao                                                 */
/*    Created:      Fri Aug 28 2020                                           */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Controller1          controller                    
// DriveBL              motor         1               
// DriveBR              motor         2               
// DriveFL              motor         8               
// DriveFR              motor         4               
// flyL                 motor         6               
// flyR                 motor         7               
// intake               motor         9               
// rotate               motor         12              
// indexer              digital_out   A               
// aimVision            vision        13              
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"
// #include <cmath>
// #include <math.h>

struct asyncParams {
  int timeDelay;
  motor *toStop;
};

using namespace vex;
competition Competition;

# define mPi 3.14159265358979323846
# define tile 23.5
# define alignTile 3

// ---- USER INPUT VARIABLES ----
int j = 1; //time per blink
int m = 60; // time per meow
int driveThreshold = 10;
int turnThreshold = 10;

float slowMult = 2.4;
float normMult = 6;

int maxFlySpeedL = 240;
int maxFlySpeedR = 220;

int lowFlySpeedL = 180;
int lowFlySpeedR = 160;

float minRotSpeed = 5;
float rotAutoPow = 1;
int minDistPix = 3;
int minDegReset = 1;

int minRotDeg = -20;
int maxRotDeg = 20;

float pastOrigBuffer[10]; //length is buffer size
int bufferSize = sizeof(pastOrigBuffer) / sizeof(float);

double wheelDiam = 2.5; //inches
double chassisWidth = 20.5; //inches

// double trackDiam = 3.26; //inches
// double trackWidth = 14.5; //inches


// ---- PROGRAM VARIABLES ----

float multiplier = normMult;

bool blinked = false; //whether is blinking
bool opened = false; //whether is meowing

bool slowmode = false;
bool convMode = false;
bool aPressed = false;

long cycles = 0;

float speedL = 0;
float speedR = 0;

bool accellingL = true;
bool accellingR = true;

int goalSpeedL = lowFlySpeedL;
int goalSpeedR = lowFlySpeedR;
bool highSpeed = false;
bool speedTogHold = false;

int amFound = 0;
float lastOrixX = -1;

// ---- FORWARD DRIVE MATH ----

double wheelCirc = wheelDiam * mPi;
double distPerWheelDeg =  wheelCirc / 360;

// double trackCirc = trackDiam * mPi;
// double distPerTrackDeg =  trackCirc / 360;

// ---- TURN DRIVE MATH ----

double turnCirc = chassisWidth * mPi;
double distPerTurnDeg = turnCirc / 360;
double wheelDegPerTurnDeg = distPerTurnDeg/distPerWheelDeg;

// double trackTurnCirc = trackWidth * mPi;
// double distPerTrackTurnDeg = trackTurnCirc / 360;
// double trackDegPerTurnDeg = distPerTrackTurnDeg/distPerTrackDeg;

// ---- AUTON ----

double scale (double inSt, double inEn, double outSt, double outEn, double val){
  return (val-inSt)/(inEn-inSt) * (outEn-outSt) + outSt;
}

void turnDegsCW(int speed, int degs){
  DriveFL.spinFor(degs*wheelDegPerTurnDeg*2, deg, speed, rpm, false);
  DriveBL.spinFor(degs*wheelDegPerTurnDeg*2, deg, speed, rpm, false);
  DriveFR.spinFor(-degs*wheelDegPerTurnDeg*2, deg, speed, rpm, false);
  DriveBR.spinFor(-degs*wheelDegPerTurnDeg*2, deg, speed, rpm);
}

int controlAxis (int axis){
  switch(axis){
    case 1: { return Controller1.Axis1.position();}
    case 2: { return Controller1.Axis2.position();}
    case 3: { return Controller1.Axis3.position();}
    case 4: { return Controller1.Axis4.position();}
    default: return 0;
  }
}

bool controlButton (char button, bool top = false){
  switch(button){
    case 'A':{ return Controller1.ButtonA.pressing();}
    case 'B':{ return Controller1.ButtonB.pressing();}
    case 'X':{ return Controller1.ButtonX.pressing();}
    case 'Y':{ return Controller1.ButtonY.pressing();}

    case 'U':{ return Controller1.ButtonUp.pressing();}
    case 'D':{ return Controller1.ButtonDown.pressing();}
    case 'L':{ return Controller1.ButtonLeft.pressing();}
    case 'R':{ return Controller1.ButtonRight.pressing();}

    case 'l':{
      if(top) return Controller1.ButtonL1.pressing();
      else return Controller1.ButtonL2.pressing();
    }
    case 'r':{
      if(top) return Controller1.ButtonR1.pressing();
      else return Controller1.ButtonR2.pressing();
    }

    default:
      return false;
  }
}

void stopDrive(int motorr){
  //stops driving
  switch(motorr){
    case 1: DriveBL.stop(); break;
    case 2: DriveBR.stop(); break;

    case 3: DriveFL.stop(); break;
    case 4: DriveFR.stop(); break;
  }
}

void drawFace (const char * bg, const char * linee) {
  int v1 = rand() % 10;
  int v2 = rand() % 60;
  
  if (v1 == 0 && !blinked){
      
    blinked = true;
    
    Brain.Screen.setPenColor(bg);
    
    Brain.Screen.drawRectangle(1,1,400,75,bg);
    
    for (int i = 0; i < 3; i++){
      Brain.Screen.setPenColor(linee);
      
      Brain.Screen.drawLine(80,50+i,120,50+i);
      Brain.Screen.drawLine(280,50+i,320,50+i);
    }
    
    j = 2;
      
  }else if (blinked){
    if (j <= 0){
      Brain.Screen.setPenColor(bg);
      
      Brain.Screen.drawRectangle(1,1,400,75,bg);
      Brain.Screen.drawCircle(100,50,20,linee);
      Brain.Screen.drawCircle(300,50,20,linee);

      blinked = false;
    }else {
      j--;
    }
  }
  
  if (v2 == 0 && !opened){
      
    opened = true;
    
    Brain.Screen.setPenColor(bg);
    
    Brain.Screen.drawRectangle(130,80,271-130,170-80,bg);
    
    for(int i = 0; i < 10; i ++) {
      Brain.Screen.setPenColor(linee);

      Brain.Screen.drawLine(130,100 + i,165,150 + i);
      Brain.Screen.drawLine(165,150 + i,200,120 + i);
      Brain.Screen.drawLine(200,120 + i,235,150 + i);
      Brain.Screen.drawLine(235,150 + i,270,100 + i);
    }
    Brain.Screen.setPenColor(linee);
    Brain.Screen.printAt(280,125,"- Meow");

    m = 4;
      
  }else if (opened){
    if (m <= 0){
      Brain.Screen.setPenColor(bg);
      
      Brain.Screen.drawRectangle(130,80,271,170,bg);
      for(int i = 0; i < 10; i ++) {
        Brain.Screen.setPenColor(linee);

        Brain.Screen.drawLine(130,100 + i,165,150 + i);
        Brain.Screen.drawLine(165,150 + i,200,120 + i);
        Brain.Screen.drawLine(200,120 + i,235,150 + i);
        Brain.Screen.drawLine(235,150 + i,270,100 + i);
      }
      opened = false;
    }else {
      if (m % 1 == 0 && m > 2){
        Brain.Screen.setPenColor(bg);
        
        Brain.Screen.drawRectangle(130,80,271-130,170-80,bg);
        int n = (4 - m)*3;
        
        for(int i = 0; i < 5; i ++) {
          Brain.Screen.setPenColor(linee);

          Brain.Screen.drawLine(130,100 + i,165,150 + i - n);
          Brain.Screen.drawLine(165,150 + i - n,200,120 + i - ceil(n/2));
          Brain.Screen.drawLine(200,120 + i - ceil(n/2),235,150 + i - n);
          Brain.Screen.drawLine(235,150 + i - n,270,100 + i);
        }
        for(int i = 5; i < 10; i ++) {
          Brain.Screen.setPenColor(linee);

          Brain.Screen.drawLine(130,100 + i,165,150 + i);
          Brain.Screen.drawLine(165,150 + i,200,120 + i);
          Brain.Screen.drawLine(200,120 + i,235,150 + i);
          Brain.Screen.drawLine(235,150 + i,270,100 + i);
        }
      }else if (m % 1 == 0 && m <= 2){
        Brain.Screen.setPenColor(bg);
        
        Brain.Screen.drawRectangle(130,80,271-130,170-80,bg);
        
        int n = m *3;
        
        for(int i = 0; i < 5; i ++) {
          Brain.Screen.setPenColor(linee);

          Brain.Screen.drawLine(130,100 + i,165,150 + i - n);
          Brain.Screen.drawLine(165,150 + i - n,200,120 + i - ceil(n/2));
          Brain.Screen.drawLine(200,120 + i - ceil(n/2),235,150 + i - n);
          Brain.Screen.drawLine(235,150 + i - n,270,100 + i);
        }
        for(int i = 5; i < 10; i ++) {
          Brain.Screen.setPenColor(linee);

          Brain.Screen.drawLine(130,100 + i,165,150 + i);
          Brain.Screen.drawLine(165,150 + i,200,120 + i);
          Brain.Screen.drawLine(200,120 + i,235,150 + i);
          Brain.Screen.drawLine(235,150 + i,270,100 + i);
        }
      }
      m--;
    }
  }
}

void drive(int speed1, int speed2, int speed3, int speed4){
  float res1 = fmax(fmin(speed1, 100),-100);
  float res2 = fmax(fmin(speed2, 100),-100);
  float res3 = fmax(fmin(speed3, 100),-100);
  float res4 = fmax(fmin(speed4, 100),-100);

  if(speed1 == 0) {
    stopDrive(1);
  }else {
    DriveBL.spin(forward,res1 * multiplier,rpm);
  }

  if(speed2 == 0) {
    stopDrive(2);
  }else {
    DriveBR.spin(forward,res2 * multiplier,rpm);
  }

  if(speed3 == 0) {
    stopDrive(3);
  }else {
    DriveFL.spin(forward,res3 * multiplier,rpm);
  }

  if(speed4 == 0) {
    stopDrive(4);
  }else {
    DriveFR.spin(forward,res4 * multiplier,rpm);
  }
}

int asyncStop (void* args) {
  asyncParams *syncs = (asyncParams *) args;
  vex::task::sleep(syncs->timeDelay);
  syncs->toStop->stop();
  return 0;
}

void initialize () {
  for (int i = 0; i < bufferSize; i++){
    pastOrigBuffer[i] = -1;
  }

  DriveFL.setBrake(brake);
  DriveFR.setBrake(brake);
  DriveBL.setBrake(brake);
  DriveBR.setBrake(brake);

  flyL.setBrake(coast);
  flyR.setBrake(coast);

  rotate.setBrake(hold);
  intake.setBrake(coast);

  rotate.resetPosition();

  Brain.Screen.setPenColor("#FFFFFF");
  Brain.Screen.drawRectangle(0, 0, 500, 500, "#000000");
  Brain.Screen.drawCircle(100,50,20,"#FFFFFF");
  Brain.Screen.drawCircle(300,50,20,"#FFFFFF");

  for(int i = 0; i < 10; i ++) {
    Brain.Screen.drawLine(130,100 + i,165,150 + i);
    Brain.Screen.drawLine(165,150 + i,200,120 + i);
    Brain.Screen.drawLine(200,120 + i,235,150 + i);
    Brain.Screen.drawLine(235,150 + i,270,100 + i);
  }

  while(true){
    drawFace("#000000", "#FFFFFF");

    vex::task::sleep(100);
  }
}

void driveLoop () {

  int driveAms[] = {0,0,0,0};
  float speedRot = 0;

  // ---- DRIVING ----

  if(controlAxis(3) > driveThreshold || controlAxis(3) < -driveThreshold){
    driveAms[0] += controlAxis(3);
    driveAms[1] += controlAxis(3);
    driveAms[2] += controlAxis(3);
    driveAms[3] += controlAxis(3);
  }

  if(controlAxis(4) > driveThreshold || controlAxis(4) < -driveThreshold){
    driveAms[0] -= controlAxis(4);
    driveAms[1] += controlAxis(4);
    driveAms[2] += controlAxis(4);
    driveAms[3] -= controlAxis(4);
  }

  if(controlAxis(1) > turnThreshold || controlAxis(1) < -turnThreshold){
    driveAms[0] += controlAxis(1);
    driveAms[1] -= controlAxis(1);
    driveAms[2] += controlAxis(1);
    driveAms[3] -= controlAxis(1);

    if (controlButton('l', false)){
      speedRot -= controlAxis(1)*multiplier/4;
    }
  }

  drive(driveAms[0],driveAms[1],driveAms[2],driveAms[3]);

  if(controlButton('l', true)){
    intake.stop();
  }else {
    intake.spin(forward, 200, rpm);
  }

  if(controlButton('r', false)){
    indexer.set(true);
  }else {
    indexer.set(false);
  }

  if(controlButton('r', true)){
    intake.spin(reverse, 200, rpm);
  }

  if(controlButton('D', false)){
    goalSpeedL = 0;
    goalSpeedR = 0;
  }else {
    if (highSpeed){
      goalSpeedL = maxFlySpeedL;
      goalSpeedR = maxFlySpeedR;
    }else {
      goalSpeedL = lowFlySpeedL;
      goalSpeedR = lowFlySpeedR;
    }
  }

  if(controlButton('L', true) && !speedTogHold){
    if(highSpeed){
      goalSpeedL = lowFlySpeedL;
      goalSpeedR = lowFlySpeedR;
      accellingL = true;
      accellingR = true;
      highSpeed = false;
      Controller1.rumble(".");
    } else{
      goalSpeedL = maxFlySpeedL;
      goalSpeedR = maxFlySpeedR;
      accellingL = true;
      accellingR = true;
      highSpeed = true;
      Controller1.rumble("-");
    }
    speedTogHold = true;
  }else if (!controlButton('L', true)){
    speedTogHold = false;
  }

  if (accellingL){
    if (speedL < goalSpeedL){
      speedL += 10;
      if (speedL >= goalSpeedL){
        accellingL = false;
      }
    }else {
      speedL -= 10;
      if (speedL <= goalSpeedL){
        accellingL = false;
      }
    }
  }
  if (accellingR){
    if (speedR < goalSpeedR){
      speedR += 10;
      if (speedR >= goalSpeedR){
        accellingR = false;
      }
    }else {
      speedR -= 10;
      if (speedR <= goalSpeedR){
        accellingR = false;
      }
    }
  }

  flyL.spin(forward, speedL, rpm);
  flyR.spin(forward, speedR, rpm);

  Controller1.Screen.clearScreen();
  Controller1.Screen.setCursor(0, 1);
  Controller1.Screen.print(speedL);
  Controller1.Screen.setCursor(0, 20);
  Controller1.Screen.print(speedR);

  bool found = true;
  float origX = 315/2;

  aimVision.takeSnapshot(aimVision__BLUE_GOAL);
  if(aimVision.largestObject.exists && aimVision.largestObject.width > 4) {
    origX = aimVision.largestObject.originX;
  }else {
    aimVision.takeSnapshot(aimVision__RED_GOAL);
    if(aimVision.largestObject.exists) {
      origX = aimVision.largestObject.originX;
    }else {
      found = false;
    }
  }

  for (int i = 0; i < bufferSize-1; i++){
    pastOrigBuffer[i] = pastOrigBuffer[i+1];
  }

  pastOrigBuffer[bufferSize-1] = found?origX:-1;

  float dOrigX = 0;
  int amVal = 0;

  for (int i = 0; i < bufferSize; i++){
    if (pastOrigBuffer[i] != -1) {
      amVal+=(i+1)/2;
      dOrigX += pastOrigBuffer[i]*(i+1)/2;
    }
  }
  dOrigX /= amVal;

  bool toCenter = false;

  if (controlButton('l', false)) {
    if (found) {
    amFound = 5;
      if (dOrigX > 316/2 && dOrigX - 316/2 > minDistPix) {
        speedRot += fmax(powf((dOrigX - 316/2)/15.5, 2) + 10, minRotSpeed);
      }else if (316/2 - dOrigX > minDistPix) {
        speedRot -= fmax(powf((316/2 - dOrigX)/15.5, 2) + 10, minRotSpeed);
      }else {
      }
    }else if (amFound >= 0){
      amFound--;
    }else {
      toCenter = true;
    }
  }else {
    toCenter = true;
  }

  if (toCenter) {
    if (rotate.position(deg) > minDegReset*5){
      speedRot -= fmax(rotate.position(deg)/2, minRotSpeed);
    }else if (rotate.position(deg) < -minDegReset*5) {
      speedRot += fmax(-rotate.position(deg)/2, minRotSpeed);;
    }else {
      speedRot = 0;
    }
  }

  for (int i = 0; i < bufferSize; i++){
    pastOrigBuffer[i] = -1;
  }

  speedRot = fmax(fmin(speedRot, 200),-200);

  if(speedRot > 0 && rotate.position(deg) < maxRotDeg*5) {
    rotate.spin(forward, speedRot, rpm);
  }else if (speedRot < 0 && rotate.position(deg) > minRotDeg*5) {
    rotate.spin(forward, speedRot, rpm);
  }else {
    rotate.stop();
  }

  // ---- SLOWMODE ----

  if(controlButton('A', true) && !aPressed){
    if(slowmode){
      multiplier = normMult;
      slowmode = false;
      Controller1.rumble(".");
    }else {
      multiplier = slowMult;
      slowmode = true;
      Controller1.rumble("..");
    }
    aPressed = true;
  }else if (!controlButton('A', true)){
    aPressed = false;
  }

  cycles++;

  driveLoop();
}

void expand (int speedLin, int speedRin) {
  accellingL = true;
  accellingR = true;
  
  goalSpeedL = speedLin;
  goalSpeedR = speedRin;

  while(accellingL || accellingR){
    if (accellingL){
      if (speedL < goalSpeedL){
        speedL += 10;
        if (speedL >= goalSpeedL){
          accellingL = false;
        }
      }else {
        speedL -= 10;
        if (speedL <= goalSpeedL){
          accellingL = false;
        }
      }
    }
    if (accellingR){
      if (speedR < goalSpeedR){
        speedR += 10;
        if (speedR >= goalSpeedR){
          accellingR = false;
        }
      }else {
        speedR -= 10;
        if (speedR <= goalSpeedR){
          accellingR = false;
        }
      }
    }
    flyL.spin(forward, speedL, rpm);
    flyR.spin(forward, speedR, rpm);

    Controller1.Screen.clearScreen();
    Controller1.Screen.setCursor(0, 1);
    Controller1.Screen.print(speedL);
    Controller1.Screen.setCursor(0, 20);
    Controller1.Screen.print(speedR);
  }
}

void doubleDropAuto (){
  expand(lowFlySpeedL, lowFlySpeedR);

  indexer.set(true);

  vex::task::sleep(1000);

  indexer.set(false);

  intake.spin(forward, 200, rpm);

  vex::task::sleep(5000);

  indexer.set(true);

  vex::task::sleep(1000);

  indexer.set(false);
}

void hailMaryAuto (){
  expand(300, 280);

  int start = vex::timer::system();

  int amTime = 500;

  while (vex::timer::system() - start <= amTime) {
    bool found = true;
    float origX = 315/2;

    aimVision.takeSnapshot(aimVision__BLUE_GOAL);
    if(aimVision.largestObject.exists && aimVision.largestObject.width > 4) {
      origX = aimVision.largestObject.originX;
    }else {
      aimVision.takeSnapshot(aimVision__RED_GOAL);
      if(aimVision.largestObject.exists) {
        origX = aimVision.largestObject.originX;
      }else {
        found = false;
      }
    }
    vex::task::sleep(10);
  }

  indexer.set(true);

  vex::task::sleep(1000);

  indexer.set(false);

  intake.spin(forward, 200, rpm);

  turnDegsCW(100, 150);

  vex::task::sleep(1000);

  expand(lowFlySpeedL, lowFlySpeedR);

  indexer.set(true);

  vex::task::sleep(1000);

  indexer.set(false);

  accellingL = true;
  accellingR = true;
  goalSpeedL = lowFlySpeedL;
  goalSpeedR = lowFlySpeedR;
}

int main() {

  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();

  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(doubleDropAuto);
  // Competition.autonomous(hailMaryAuto);
  Competition.drivercontrol(driveLoop);

  // Run the pre-autonomous function.
  initialize();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}
