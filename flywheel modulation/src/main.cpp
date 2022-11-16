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
// DriveL               motor         1               
// DriveR               motor         5               
// piston               digital_out   A               
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"
#include <cmath>

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

float slowMult = 0.8;
float normMult = 1.9;

// ---- PROGRAM VARIABLES ----

float multiplier = normMult;

bool blinked = false; //whether is blinking
bool opened = false; //whether is meowing

bool slowmode = false;
bool aPressed = false;
long cycles = 0;

bool accelling = true;

int speedL = 0;
int speedR = 0;
bool upLPress = false;
bool downLPress = false;
bool upRPress = false;
bool downRPress = false;

long ULC = 0;
long DLC = 0;
long URC = 0;
long DRC = 0;

int controlAxis (int axis){
  switch(axis){
    case 1: {
      return Controller1.Axis1.position();
    }
    case 2: {
      return Controller1.Axis2.position();
    }
    case 3: {
      return Controller1.Axis3.position();
    }
    case 4: {
      return Controller1.Axis4.position();
    }

    default:
      return 0;
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

void drawFace (const char * bg, const char * linee) {
  int v1 = rand() % 10;
  int v2 = rand() % 60;
  
  if (v1 == 0 && !blinked){
      
    blinked = true;
    
    Brain.Screen.setPenColor(bg);
    
    Brain.Screen.drawRectangle(1,1,400,75,bg);
    
    //Brain.Screen.drawCircle(100,50,20,"#FFFFFF");
    //Brain.Screen.drawCircle(300,50,20,"#FFFFFF");
    
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

void initialize () {

  DriveL.setBrake(vex::brakeType::coast);
  DriveR.setBrake(vex::brakeType::coast);

  while(true){
    drawFace("#00719e", "#FFFFFF");
    vex::task::sleep(100);
  }
}

void driveLoop () {

  // ---- DRIVING ----

  if (accelling){
    speedL += 10;
    speedR += 10;
    if (speedL >= 600){
      accelling = false;
    }
  }else {
    if(controlButton('U', true) && !upLPress){
      speedL += 5;
      ULC = cycles;
      upLPress = true;
    }else if (!controlButton('U', true)){
      upLPress = false;
    }

    if(controlButton('D', true) && !downLPress){
      speedL -= 5;
      downLPress = true;
    }else if (!controlButton('D', true)){
      downLPress = false;
    }

    if(controlButton('X', true) && !upRPress){
      speedR += 5;
      upRPress = true;
    }else if (!controlButton('X', true)){
      upRPress = false;
    }

    if(controlButton('B', true) && !downRPress){
      speedR -= 5;
      downRPress = true;
    }else if (!controlButton('B', true)){
      downRPress = false;
    }
  }

  if(controlButton('r', true)){
    piston.set(true);
  }

  if(controlButton('r', false)){
    piston.set(false);
  }

  Controller1.Screen.setCursor(0, 1);
  Controller1.Screen.print(speedL);
  Controller1.Screen.setCursor(0, 20);
  Controller1.Screen.print(speedR);

  DriveL.spin(fwd, speedL, rpm);
  DriveR.spin(fwd, speedR, rpm);

  // ---- SLOWMODE ----

  if(controlButton('A', true) && !aPressed){
    if(slowmode){
      multiplier = normMult;
      slowmode = false;
    }else {
      multiplier = slowMult;
      slowmode = true;
    }
    aPressed = true;
  }else if (!controlButton('A', true)){
    aPressed = false;
  }

  cycles++;

  driveLoop();
}

int main() {

  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();

  // cycleBallsInt(2, 1000);

  // Set up callbacks for autonomous and driver control periods.
  // skillAutonomous();
  // Competition.autonomous(skillAutonomous);
  // Competition.autonomous(skillAutonomousIdiot);
  Competition.drivercontrol(driveLoop);

  // Run the pre-autonomous function.
  initialize();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}
