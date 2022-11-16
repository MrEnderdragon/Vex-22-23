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

int maxFlySpeedL = 510;
int maxFlySpeedR = 500;

float minRotSpeed = 5;
float rotAutoPow = 1;
int minDistPix = 3;

int minRotDeg = -20;
int maxRotDeg = 20;

// double wheelDiam = 2.5; //inches
// double chassisWidth = 18.5; //inches

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

int amFound = 0;
float lastOrixX = -1;

// ---- FORWARD DRIVE MATH ----

// double wheelCirc = wheelDiam * mPi;
// double distPerWheelDeg =  wheelCirc / 360;

// double trackCirc = trackDiam * mPi;
// double distPerTrackDeg =  trackCirc / 360;

// ---- TURN DRIVE MATH ----

// double turnCirc = chassisWidth * mPi;
// double distPerTurnDeg = turnCirc / 360;
// double wheelDegPerTurnDeg = distPerTurnDeg/distPerWheelDeg;

// double trackTurnCirc = trackWidth * mPi;
// double distPerTrackTurnDeg = trackTurnCirc / 360;
// double trackDegPerTurnDeg = distPerTrackTurnDeg/distPerTrackDeg;

// ---- AUTON ----

double scale (double inSt, double inEn, double outSt, double outEn, double val){
  return (val-inSt)/(inEn-inSt) * (outEn-outSt) + outSt;
}

// void driveDist(int speed, int dist){
//   DriveFL.spinFor(dist/distPerWheelDeg, deg,speed, rpm, false);
//   DriveBL.spinFor(dist/distPerWheelDeg, deg,speed, rpm, false);
//   DriveFR.spinFor(dist/distPerWheelDeg, deg,speed, rpm, false);
//   DriveBR.spinFor(dist/distPerWheelDeg, deg,speed, rpm);
// }

// double fabs(double inn){
//   if(inn < 0) {
//     return -inn;
//   }
//   return inn;
// }

// void distTrack (double dist, int speed, double accellPerc = 10, double decellPerc=80){
//   leftEnc.resetRotation();
//   rightEnc.resetRotation();

//   double prevL = 0;
//   double prevR = 0;
//   int moveStuck = 0;

//   double distDegs = dist/distPerTrackDeg;
//   decellPerc /= 100;
//   accellPerc /= 100;

//   while(true){
//     double posL = leftEnc.position(deg);
//     double posR = rightEnc.position(deg);

//     if(fabs(posL-prevL) < moveStuckThresh && fabs(posR-prevR) < moveStuckThresh){
//       moveStuck++;
//     }else {
//       moveStuck = 0;
//     }

//     prevL = posL;
//     prevR = posR;

//     if(moveStuck > moveStuckReps) break;

//     // Brain.Screen.print("%f", posR);

//     double speed1 = speed * (distDegs>0?1:-1);
//     double speed2 = speed * (distDegs>0?1:-1);

//     double percentL = fabs(posL / distDegs);
//     double percentR = fabs(posR / distDegs);

//     double speedAcc = speed1;

//     if(percentL > decellPerc){
//       speedAcc = fabs(speed1 * scale(decellPerc, 1, 1, 0, percentL)) * (distDegs>0?1:-1);
//     }else if (percentL < accellPerc){
//       speedAcc = fabs(speed1 * scale(0, accellPerc, 0, 1, percentL)) * (distDegs>0?1:-1);
//     }

//     double comp = minSpeed * ((posL < distDegs)?1:-1);
//     speed1 = (fabs(comp) > fabs(speedAcc))?comp:speedAcc;
    
//     speedAcc = speed2;

//     if(percentR > decellPerc){
//       speedAcc = fabs(speed2 * scale(decellPerc, 1, 1, 0, percentR)) * (distDegs>0?1:-1);
//     }else if (percentR < accellPerc){
//       speedAcc = fabs(speed2 * scale(0, accellPerc, 0, 1, percentR)) * (distDegs>0?1:-1);
//     }

//     comp = minSpeed * ((posR < distDegs)?1:-1);
//     speed2 = (fabs(comp) > fabs(speedAcc))?comp:speedAcc;

//     DriveFL.spin(forward, speed1, rpm);
//     DriveBL.spin(forward, speed1, rpm);

//     DriveFR.spin(forward, speed2, rpm);
//     DriveBR.spin(forward, speed2, rpm);

//     if(fabs(distDegs-posL) < 50 && fabs(distDegs-posR) < 50) break;

//     vex::task::sleep(moveAcc);
//   }
  
//   DriveFL.stop();
//   DriveBL.stop();
//   DriveFR.stop();
//   DriveBR.stop();
// }

// void distTrackSimp (double dist, int speed, double accellPerc = 10, double decellPerc=80){
//   leftEnc.resetRotation();
//   rightEnc.resetRotation();

//   double prevL = 0;
//   double prevR = 0;
//   int moveStuck = 0;

//   double distDegs = dist/distPerTrackDeg;
//   decellPerc /= 100;
//   accellPerc /= 100;

//   while(true){
//     double posL = fabs(leftEnc.position(deg));
//     double posR = fabs(rightEnc.position(deg));

//     if(fabs(posL-prevL) < moveStuckThresh && fabs(posR-prevR) < moveStuckThresh){
//       moveStuck++;
//     }else {
//       moveStuck = 0;
//     }

//     double percentL = fabs(posL / distDegs);
//     double percentR = fabs(posR / distDegs);

//     double speed1 = speed;

//     if(percentL < accellPerc){
//       speed1 = scale(0, accellPerc, minSpeed, speed, percentL);
//     }else if (percentL > decellPerc){
//       speed1 = scale(decellPerc, 1, speed, minSpeed, percentL);
//     }

//     double speed2 = speed;

//     if(percentR < accellPerc){
//       speed2 = scale(0, accellPerc, minSpeed, speed, percentR);
//     }else if (percentR > decellPerc){
//       speed2 = scale(decellPerc, 1, speed, minSpeed, percentR);
//     }

//     speed1 = fabs(speed1);
//     speed2 = fabs(speed2);

//     if (percentL > 1) speed1 = 0;
//     if (percentR > 1) speed2 = 0;

//     prevL = posL;
//     prevR = posR;

//     if(moveStuck > moveStuckReps) break;
      
//     if (dist >= 0){
//       DriveFL.spin(forward, speed1, rpm);
//       DriveBL.spin(forward, speed1, rpm);

//       DriveFR.spin(forward, speed2, rpm);
//       DriveBR.spin(forward, speed2, rpm);
//     }else {
//       DriveFL.spin(reverse, speed1, rpm);
//       DriveBL.spin(reverse, speed1, rpm);

//       DriveFR.spin(reverse, speed2, rpm);
//       DriveBR.spin(reverse, speed2, rpm);
//     }

//     if(percentL >= 1 && percentR >= 1) break;

//     vex::task::sleep(moveAcc);
//   }
  
//   DriveFL.stop();
//   DriveBL.stop();
//   DriveFR.stop();
//   DriveBR.stop();
// }

// void turnDegsCW(int speed, int degs){
//   DriveFL.spinFor(degs*wheelDegPerTurnDeg, deg, speed, rpm, false);
//   DriveBL.spinFor(degs*wheelDegPerTurnDeg, deg, speed, rpm, false);
//   DriveFR.spinFor(-degs*wheelDegPerTurnDeg, deg, speed, rpm, false);
//   DriveBR.spinFor(-degs*wheelDegPerTurnDeg, deg, speed, rpm);
// }

// void turnTrackCW (double degs, int speed, double accellPerc = 10, double decellPerc=80){
//   leftEnc.resetRotation();
//   rightEnc.resetRotation();

//   double distDegsL = degs*trackDegPerTurnDeg;
//   double distDegsR = -degs*trackDegPerTurnDeg;
//   decellPerc /= 100;
//   accellPerc /= 100;

//   while(true){
//     double speed1 = speed * (degs<0?-1:1);
//     double speed2 = -speed * (degs<0?-1:1);

//     double posL = leftEnc.position(deg);
//     double posR = rightEnc.position(deg);

//     double percentL = posL / distDegsL;
//     double percentR = posR / distDegsR;

//     double speedAcc = speed1;

//     if(percentL > decellPerc){
//       speedAcc = fabs(speed1 * scale(decellPerc, 1, 1, 0, percentL)) * (distDegsL>0?1:-1);;
//     }else if (percentL < accellPerc){
//       speedAcc = fabs(speed1 * scale(0, accellPerc, 0, 1, percentL)) * (distDegsL>0?1:-1);;
//     }
//     double comp = minSpeed * (posL < distDegsL?1:-1);
//     speed1 = fabs(comp) > fabs(speedAcc)?comp:speedAcc;

//     speedAcc = speed2;

//     if(percentR > decellPerc){
//       speedAcc = fabs(speed2 * scale(decellPerc, 1, 1, 0, percentR)) * (distDegsR>0?1:-1);;
//     }else if (percentR < accellPerc){
//       speedAcc = fabs(speed2 * scale(0, accellPerc, 0, 1, percentR)) * (distDegsR>0?1:-1);;
//     }

//     comp = minSpeed * (posR < distDegsR?1:-1);
//     speed2 = fabs(comp) > fabs(speedAcc)?comp:speedAcc;

//     DriveFL.spin(forward, speed1, rpm);
//     DriveBL.spin(forward, speed1, rpm);

//     DriveFR.spin(forward, speed2, rpm);
//     DriveBR.spin(forward, speed2, rpm);

//     if(fabs(distDegsL-posL) < 50 && fabs(distDegsR-posR) < 50) break;

//     vex::task::sleep(moveAcc);
//   }

//   DriveFL.stop();
//   DriveBL.stop();
//   DriveFR.stop();
//   DriveBR.stop();
// }

// void turnTrackCWSimp (double degs, int speed, double accellPerc = 10, double decellPerc=80){
//   leftEnc.resetRotation();
//   rightEnc.resetRotation();

//   double prevL = 0;
//   double prevR = 0;
//   int moveStuck = 0;

//   double distDegs = degs*trackDegPerTurnDeg;
//   decellPerc /= 100;
//   accellPerc /= 100;

//   while(true){
//     double posL = fabs(leftEnc.position(deg));
//     double posR = fabs(rightEnc.position(deg));

//     if(fabs(posL-prevL) < moveStuckThresh && fabs(posR-prevR) < moveStuckThresh){
//       moveStuck++;
//     }else {
//       moveStuck = 0;
//     }

//     double percentL = fabs(posL / distDegs);
//     double percentR = fabs(posR / distDegs);

//     double speed1 = speed;

//     if(percentL < accellPerc){
//       speed1 = scale(0, accellPerc, minSpeed, speed, percentL);
//     }else if (percentL > decellPerc){
//       speed1 = scale(decellPerc, 1, speed, minSpeed, percentL);
//     }

//     double speed2 = speed;

//     if(percentR < accellPerc){
//       speed2 = scale(0, accellPerc, minSpeed, speed, percentR);
//     }else if (percentR > decellPerc){
//       speed2 = scale(decellPerc, 1, speed, minSpeed, percentR);
//     }

//     speed1 = fabs(speed1);
//     speed2 = fabs(speed2);

//     if (percentL > 1) speed1 = 0;
//     if (percentR > 1) speed2 = 0;

//     prevL = posL;
//     prevR = posR;

//     if(moveStuck > moveStuckReps) break;
      
//     if (degs >= 0){
//       DriveFL.spin(forward, speed1, rpm);
//       DriveBL.spin(forward, speed1, rpm);

//       DriveFR.spin(reverse, speed2, rpm);
//       DriveBR.spin(reverse, speed2, rpm);
//     }else {
//       DriveFL.spin(reverse, speed1, rpm);
//       DriveBL.spin(reverse, speed1, rpm);

//       DriveFR.spin(forward, speed2, rpm);
//       DriveBR.spin(forward, speed2, rpm);
//     }

//     if(percentL >= 1 && percentR >= 1) break;

//     vex::task::sleep(moveAcc);
//   }
  
//   DriveFL.stop();
//   DriveBL.stop();
//   DriveFR.stop();
//   DriveBR.stop();
// }

// void driveTime (int speed, double amTime){

//   amTime *= 1000;

//   int start = vex::timer::system();

//   DriveBL.spin(forward, speed, rpm);
//   DriveBR.spin(forward, speed, rpm);
//   DriveFL.spin(forward, speed, rpm);
//   DriveFR.spin(forward, speed, rpm);

//   while(vex::timer::system() - start <= amTime){
//     vex::task::sleep(moveAcc);
//   }

//   DriveFL.stop();
//   DriveFR.stop();
//   DriveBL.stop();
//   DriveBR.stop();
// }

// void driveTime2 (int speedL, int speedR, double amTime){

//   amTime *= 1000;

//   int start = vex::timer::system();

//   DriveBL.spin(forward, speedL, rpm);
//   DriveBR.spin(forward, speedR, rpm);
//   DriveFL.spin(forward, speedL, rpm);
//   DriveFR.spin(forward, speedR, rpm);

//   while(vex::timer::system() - start <= amTime){
//     vex::task::sleep(moveAcc);
//   }

//   DriveFL.stop();
//   DriveFR.stop();
//   DriveBL.stop();
//   DriveBR.stop();
// }

// void driveTime3 (int speed1, int speed2, int speed3, int speed4, double amTime){

//   amTime *= 1000;

//   int start = vex::timer::system();

//   DriveBL.spin(forward, speed1, rpm);
//   DriveBR.spin(forward, speed2, rpm);
//   DriveFL.spin(forward, speed3, rpm);
//   DriveFR.spin(forward, speed4, rpm);

//   while(vex::timer::system() - start <= amTime){
//     vex::task::sleep(moveAcc);
//   }

//   DriveFL.stop();
//   DriveFR.stop();
//   DriveBL.stop();
//   DriveBR.stop();
// }

// void turnTime (int speed, double amTime){

//   amTime *= 1000;

//   int start = vex::timer::system();

//   DriveBL.spin(forward, speed, rpm);
//   DriveBR.spin(forward, -speed, rpm);
//   DriveFL.spin(forward, speed, rpm);
//   DriveFR.spin(forward, -speed, rpm);

//   while(vex::timer::system() - start <= amTime){
//     vex::task::sleep(moveAcc);
//   }

//   DriveFL.stop();
//   DriveFR.stop();
//   DriveBL.stop();
//   DriveBR.stop();
// }

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
  }

  drive(driveAms[0],driveAms[1],driveAms[2],driveAms[3]);

  if(controlButton('l', false)){
    intake.stop();
  }else {
    intake.spin(forward, 200, rpm);
  }

  if(controlButton('r', false)){
    indexer.set(true);
  }else {
    indexer.set(false);
  }

  if (accellingL){
    speedL += 10;
    if (speedL >= maxFlySpeedL){
      accellingL = false;
    }
  }

  if (accellingR){
    speedR += 10;
    if (speedR >= maxFlySpeedR){
      accellingR = false;
    }
  }

  flyL.spin(forward, speedL, rpm);
  flyR.spin(forward, speedR, rpm);

  bool found = true;
  float origX = 315/2;

  aimVision.takeSnapshot(aimVision__BLUE_GOAL);
  if(aimVision.largestObject.exists) {
    origX = aimVision.largestObject.originX;
  }else {
    // aimVision.takeSnapshot(aimVision__RED_GOAL);
    // if(aimVision.largestObject.exists) {
    //   origX = aimVision.largestObject.originX;
    // }else {
      found = false;
    // }
  }

  float dOrigX = origX;
  if (lastOrixX != -1){
    dOrigX += lastOrixX;
    dOrigX /= 2;
  }

  if (found) {
    amFound = 10;
    if(rotate.position(deg) < maxRotDeg*5 && rotate.position(deg) > minRotDeg*5) {
      if (dOrigX > 316/2 && dOrigX - 316/2 > minDistPix) {
        rotate.spin(forward, fmax(powf((dOrigX - 316/2)/15.5, 2), minRotSpeed), rpm);
      }else if (316/2 - dOrigX > minDistPix) {
        rotate.spin(reverse, fmax(powf((316/2 - dOrigX)/15.5, 2), minRotSpeed), rpm);
      }else {
        rotate.stop();
      }
    }else {
      rotate.stop();
    }
  }else if (amFound >= 0){
    amFound--;
  }else {
    if (rotate.position(deg) > 0){
      rotate.spin(reverse, 30, rpm);
    }else if (rotate.position(deg) < 0) {
      rotate.spin(forward, 30, rpm);
    }else {
      rotate.stop();
    }
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

void expand () {

}

int main() {

  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();

  // Set up callbacks for autonomous and driver control periods.
  // Competition.autonomous(trackAutoLSkills);

  Competition.drivercontrol(driveLoop);

  // Run the pre-autonomous function.
  initialize();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}
