#include "vex.h"

using namespace vex;
using signature = vision::signature;
using code = vision::code;

// A global instance of brain used for printing to the V5 Brain screen
brain  Brain;

// VEXcode device constructors
controller Controller1 = controller(primary);
motor DriveBL = motor(PORT1, ratio6_1, false);
motor DriveBR = motor(PORT2, ratio6_1, true);
motor DriveFL = motor(PORT8, ratio6_1, false);
motor DriveFR = motor(PORT4, ratio6_1, true);
motor flyL = motor(PORT6, ratio6_1, true);
motor flyR = motor(PORT7, ratio6_1, false);
motor intake = motor(PORT9, ratio18_1, false);
motor rotate = motor(PORT12, ratio18_1, false);
digital_out indexer = digital_out(Brain.ThreeWirePort.A);
/*vex-vision-config:begin*/
signature aimVision__RED_GOAL = signature (1, 8777, 9199, 8988, -1391, -885, -1138, 10.5, 0);
signature aimVision__BLUE_GOAL = signature (2, -2881, -1883, -2382, 7695, 9793, 8744, 7, 0);
vision aimVision = vision (PORT13, 50, aimVision__RED_GOAL, aimVision__BLUE_GOAL);
/*vex-vision-config:end*/

// VEXcode generated functions
// define variable for remote controller enable/disable
bool RemoteControlCodeEnabled = true;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void vexcodeInit( void ) {
  // nothing to initialize
}