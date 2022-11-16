using namespace vex;

extern brain Brain;

using signature = vision::signature;

// VEXcode devices
extern controller Controller1;
extern motor DriveBL;
extern motor DriveBR;
extern motor DriveFL;
extern motor DriveFR;
extern motor flyL;
extern motor flyR;
extern motor intake;
extern motor rotate;
extern digital_out indexer;
extern signature aimVision__RED_GOAL;
extern signature aimVision__BLUE_GOAL;
extern signature aimVision__SIG_3;
extern signature aimVision__SIG_4;
extern signature aimVision__SIG_5;
extern signature aimVision__SIG_6;
extern signature aimVision__SIG_7;
extern vision aimVision;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void  vexcodeInit( void );