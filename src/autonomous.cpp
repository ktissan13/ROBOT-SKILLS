#include "main.h"

//*********************************AUTONMOUS CODES
void Eight_Point_Auton(){
    // Starts Intake
     intake(200);
    // Get the cubes
    slowDrive(1.38 TL);
    pros::delay(750);
      intake(0);

    turn(38);
  //
   drive(-1.25 TL);
   pros::delay(20);
   turn(-41);
  pros::delay(500);
  intake(190);
  _driveReset();
  pros::delay(500);
  slowDrive(1.3 TL);
  intake(40);
    _driveReset();
    drive(-1.0 TL);
  turn(-80);
      _driveReset();
    drive(.5 TL);
}

void Eight_Point_Auton_blue(){
    deploy(520,100);
     // Starts Intake
     intake(200);
     // Get the cubes
     slowDrive(1.2 TL);
     intake(0);
     turn(45);
     drive(-1.2 TL);
     turn(-44);
     intake(200);
     _driveReset();
     delay(100);
     slowDrive(1.45 TL);
     /*intake(19);
     _driveReset();
     drive(-1.0 TL);
     _driveReset();
     turn(-160);
     _driveReset();
     turn(20);
     _driveReset();
     drive(1.5 TL);
     intake(0);
     move_tray(560,70);
     drive(-1 TL);*/
}

//*********************************AUTONMOUS MODE
void autonomous() {
auton_mode = true;
//red is false and blue is true
mirror = true;
_driveReset(); // reset the drive encoders
reset_motors();
Task drive_task(driveTask);
Task turn_task(turnTask);
Task tray_task(trayTask);
Task lift_task(liftTask);
Task intake_task(intakeTask);

Eight_Point_Auton_blue();

drive_task.remove();
turn_task.remove();
tray_task.remove();
lift_task.remove();
intake_task.remove();
}
