#include "main.h"
#define MAX 200;
pros::Controller master(pros::E_CONTROLLER_MASTER);
pros::Controller partner(pros::E_CONTROLLER_PARTNER);
//*********************************MOTOR SETUP
Motor Chasis_L1 (19, MOTOR_GEARSET_18, false, MOTOR_ENCODER_DEGREES);
Motor Chasis_L2 (20, MOTOR_GEARSET_18, false, MOTOR_ENCODER_DEGREES);
Motor Chasis_R1 (7, MOTOR_GEARSET_18, true, MOTOR_ENCODER_DEGREES);
Motor Chasis_R2 (9, MOTOR_GEARSET_18, true, MOTOR_ENCODER_DEGREES);
Motor Lift(2,MOTOR_GEARSET_36,true, MOTOR_ENCODER_DEGREES);
Motor Intake_R(1,MOTOR_GEARSET_18,true, MOTOR_ENCODER_DEGREES);
Motor Intake_L(12,MOTOR_GEARSET_18,false, MOTOR_ENCODER_DEGREES);
Motor Tray(18,MOTOR_GEARSET_36, false, MOTOR_ENCODER_DEGREES);

//*********************************CONSTANT VARIABLES
 static int driveMode = 1;
 static int driveTarget = 0;
 static int turnTarget = 0;
 static int trayTarget = 0;
 static int tray_distance = 0;
 static int tray_speed = 0;
 static int liftTarget = 0;
 static int lift_distance = 0;
 static int lift_speed = 0;
 static int maxSpeed = MAX;
 static int slant = 0;





 //********************************Basic Controls
 // Controls left side of chassis
 void Chasis_left(int value){
   Chasis_L1.move(value);
   Chasis_L2.move(value);
 }

 // Controls right side of chassis
 void Chasis_right(int value){
   Chasis_R1.move(value);
   Chasis_R2.move(value);
 }

 // Clears the Motors encoders
 void _driveClr(){
   Chasis_L1.tare_position();
   Chasis_L2.tare_position();
   Chasis_R1.tare_position();
   Chasis_R2.tare_position();
 }

 // Resets the whole drive system
 void _driveReset(){
   maxSpeed = MAX;
   slant = 0;
   driveTarget = 0;
   turnTarget = 0;
   _driveClr();
   Chasis_left(0);
   Chasis_right(0);
 }

// if called returns integer value
 int drivePos(){
   return (Chasis_L1.get_position() + Chasis_R1.get_position())/2;
 }

// set the max speed for slew control
 void setSpeed(int speed){
   maxSpeed = speed;
 }

 //*********************************SLEW CONTROL
 // 9 works fine if any other number does not work
 const int accel_step = 9;
 const int deccel_step = 256; // no decel slew
 static int leftSpeed = 0;
 static int rightSpeed = 0;

 void leftSlew(int leftTarget){
   int step;

   if(abs(leftSpeed) < abs(leftTarget))
     step = accel_step;
   else
     step = deccel_step;

   if(leftTarget > leftSpeed + step)
     leftSpeed += step;
   else if(leftTarget < leftSpeed - step)
     leftSpeed -= step;
   else
     leftSpeed = leftTarget;

   Chasis_left(leftSpeed);
 }

 //slew control
 void rightSlew(int rightTarget){
   int step;

   if(abs(rightSpeed) < abs(rightTarget))
     step = accel_step;
   else
     step = deccel_step;

   if(rightTarget > rightSpeed + step)
     rightSpeed += step;
   else if(rightTarget < rightSpeed - step)
     rightSpeed -= step;
   else
     rightSpeed = rightTarget;

   Chasis_right(rightSpeed);
 }

//**************************************************SLOP CORRECTION
 void slop(int sp){
   driveMode = 2;
   if(sp < 0){
     Chasis_right(-30);
     pros::delay(100);
   }
   driveMode = 1;

 }

 //**************************************************FEEDBACK
 bool isDriving(){
   static int count = 0;
   static int last = 0;
   static int lastTarget = 0;

   int leftPos = Chasis_L1.get_position();
   int rightPos = Chasis_R1.get_position();

   int curr = (abs(leftPos) + abs(rightPos))/2;
   int thresh = 3;
   int target = turnTarget;

   if(driveMode == 1)
     target = driveTarget;


   if(abs(last-curr) < thresh)
     count++;
   else
     count = 0;

   if(target != lastTarget)
     count = 0;

   lastTarget = target;
   last = curr;

   //not driving if we haven't moved
   if(count > 4)
     return false;
   else
     return true;

 }

 //**************************************************AUTON FUNCTIOS FOR DRIVE
 void driveAsync(int sp){
   _driveReset();
   driveTarget = sp;
   driveMode = 1;
 }

 void turnAsync(int sp){
   if(mirror)
     sp = -sp; // inverted turn for blue auton
   _driveReset();
   turnTarget = sp;
   driveMode = 0;
 }

 void drive(int sp){
   driveAsync(sp);
   pros::delay(450);
   while(isDriving()) pros::delay(20);
 }

 void turn(int sp){
   turnAsync(sp);
   pros::delay(450);
   while(isDriving()) pros::delay(20);
 }

 void slowDrive(int sp, int dp){
   driveAsync(sp);

   if(sp > 0)
     while(drivePos() < dp) delay(20);
   else
     while(drivePos() > dp) delay(20);

   setSpeed(45);
   while(isDriving()) pros::delay(20);
 }

 /**************************************************/
 //drive modifiers
 void setSlant(int s){
   if(mirror)
     s = -s;

   slant = s;
 }

 void slant_drive(int distance,int s){
   setSlant(s);
   drive(distance);
 }

 void setCurrent(int mA){
   Chasis_L1.set_current_limit(mA);
   Chasis_L2.set_current_limit(mA);
   Chasis_R1.set_current_limit(mA);
   Chasis_R2.set_current_limit(mA);
 }

 void setBrakeMode(int mode){
   pros::motor_brake_mode_e_t brakeMode;
   switch(mode){
     case 0:
       brakeMode = MOTOR_BRAKE_COAST;
       break;
     case 1:
       brakeMode = MOTOR_BRAKE_BRAKE;
       break;
     case 2:
       brakeMode = MOTOR_BRAKE_HOLD;
       break;
   }

   Chasis_L1.set_brake_mode(brakeMode);
   Chasis_L2.set_brake_mode(brakeMode);
   Chasis_R1.set_brake_mode(brakeMode);
   Chasis_R2.set_brake_mode(brakeMode);
 }
 //**************************************************PID task
 void driveTask(void* parameter){
   int prevError = 0;
   setBrakeMode(1);

   while(true){
     if(auton_mode){
     pros::delay(20);

     if(driveMode != 1)
       continue;

     int sp = driveTarget;

     double kp = .3;
     double kd = .5;

     //read sensors
     int ls = Chasis_L1.get_position();
     int rs = Chasis_R1.get_position();
     int sv = ls;

     //speed
     int error = sp-sv;
     int derivative = error - prevError;
     prevError = error;
     int speed = error*kp + derivative*kd;

     if(speed > maxSpeed)
       speed = maxSpeed;
     if(speed < -maxSpeed)
       speed = -maxSpeed;

     //set motors
     leftSlew(speed - slant);
     rightSlew(speed + slant);
   }
 }
 }

 void turnTask(void* parameter){
   int prevError;

   while(true){
     if (auton_mode){
   pros::delay(20);

     if(driveMode != 0)
       continue;

     int sp = turnTarget;

     if(sp > 0)
       sp *= 2.35;
     else
       sp *= 2.3;

     double kp = .9;
     double kd = 1.5;

     int sv = (Chasis_R1.get_position() - Chasis_L1.get_position())/2;
     int error = sp-sv;
     int derivative = error - prevError;
     prevError = error;
     int speed = error*kp + derivative*kd;

     if(speed > maxSpeed)
       speed = maxSpeed;
     if(speed < -maxSpeed)
       speed = -maxSpeed;

     leftSlew(-speed);
     rightSlew(speed);
     }
   }
 }

//*********************************LCD SCREEN

void initialize() {
pros::lcd::initialize();
pros::lcd::set_text(1, "Super Sayin");
  pros::lcd::set_text(2, "Yeah, I watch anime! - Tissan");

}

//*********************************AUTON FUNCTIONS
void reset_motors(){
  Lift.tare_position();
  Intake_L.tare_position();
  Intake_R.tare_position();
  Tray.tare_position();
}

void intake(int vel){
  Intake_L.move_velocity(vel);
  Intake_R.move_velocity(vel);
}

void move_intake(int distance, int vel){
  Intake_L.tare_position();
  Intake_R.tare_position();
  while(Intake_L.get_position() != distance){
    Intake_L.move_absolute(distance,vel);
    Intake_R.move_absolute((-1*distance),vel);
    if (Intake_L.get_position() >= (distance-5) && Intake_L.get_position() <= (distance+5)){
      break;
    }
  }
}

void move_tray(int distance, int vel){
  Tray.tare_position();
  while(Tray.get_position() != distance){
    Tray.move_absolute(distance, vel);
    if (Tray.get_position() >= (distance-5) && Tray.get_position() <= (distance+5)){
      break;
}}}

void trayAsync(int distance,int vel){
  tray_distance = distance;
  tray_speed = vel;
  trayTarget = 1;
}

void trayTask(void* parameter){
  while(true){
    if (auton_mode){
    delay(20);

    switch(trayTarget){
      case 1:
      move_tray(tray_distance, tray_speed);
      break;
    }

    trayTarget = 0;
  }
}
}

void move_lift(int distance, int vel){
  Lift.tare_position();
  while(Lift.get_position() != distance){
    Lift.move_absolute(distance, vel);
    if (Lift.get_position() >= (distance-5) && Lift.get_position() <= (distance+5)){
      break;
}}}

void liftAsync(int distance, int vel){
  lift_distance = distance;
  lift_speed = vel;
  liftTarget = 1;
}

void liftTask(void* parameter){
  while(true){
    if (auton_mode){
    delay(20);

    switch(liftTarget){
      case 1:
      move_lift(lift_distance,lift_speed);
      break;
    }
    liftTarget = 0;
    }
  }
  }
  void lift(int vel)
{
Lift.move(vel);
}
void tray1(int vel)
{
Tray.move_velocity(vel);
}
  void liftmov(){
if(partner.get_digital(DIGITAL_L1)==1){
lift(100);
}
else if(partner.get_digital(DIGITAL_L2)==1){
lift(-100);
}
else{
  lift(0);
}}

void tray(int distance, int vel)
{
  Tray.tare_position();

  while(Tray.get_position() != distance){
    Tray.move_absolute(distance, vel);
    if (Tray.get_position() >= (distance-10) && Tray.get_position() <= (distance+10)){
      break;
    }
  }
}
int first_position = 270;
int second_position=400;
int third_position=700;

void traymove(){
 if(partner.get_digital(DIGITAL_Y)){
  Tray.tare_position();
  Tray.move_velocity(120);
  intake(30);
  while(Tray.get_position()<first_position){
    pros::delay(5);
  }
  intake(0);
  Tray.move_velocity(50);
  while(Tray.get_position()<second_position){
    pros::delay(5);
  }
  Tray.move_velocity(20);
  while(Tray.get_position()<third_position){
pros::delay(5);
  }
}
else{
    Tray.move(partner.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y));
}
}
  void intakemove(){
  if(partner.get_digital(DIGITAL_R1) == 1){
    intake(200);
  }
  else if(partner.get_digital(DIGITAL_R2) == 1){
    intake(-90);
  }
  else if(partner.get_digital(DIGITAL_UP) == 1){
    intake(-200);
  }
  else if(master.get_digital(DIGITAL_R2) == 1){
    intake(5);
  }
  else{
    intake(0);
  }
}

void non_slew_drive(int distance, int vel){
  _driveReset();
  while(Chasis_R1.get_position() != distance){
    Chasis_left(vel);
    Chasis_right(vel);
    if(Chasis_R1.get_position() >= (distance-5) && Chasis_R1.get_position() <= (distance+5)){
      Chasis_left(0);
      Chasis_right(0);
      break;
    }
  }
}

void deploy(int distance, int vel){
  reset_motors();
  move_tray(240,100);
  Tray.move_velocity(4);
  move_lift(distance,vel);
  Lift.move_velocity(0);
  Tray.move_voltage(0);
}

void super_sayin(){
  // Deploy
  //deploy(400,60);
  //trayAsync(30,70);
/*  drive(0.3 TL);
  turn(69);
  drive(0.4 TL);
  turn(-69);
  //liftAsync(-400,100);
  non_slew_drive(-0.45 TL, -80);*/
  intake(600);

  //Collect first set of Cubes
//  non_slew_drive(2 TL);

  intake(0);
  /*non_slew_drive(-100, -100);
  turn(-45);
  drive(1 TL);
  move_intake(-100,100);
  move_tray(240,100);
  drive(-1 TL);
  turn(-45);
  non_slew_drive(-100, -100);

*/
}

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

void disabled() {}
void competition_initialize() {}

//*********************************AUTONMOUS MODE
void autonomous() {
auton_mode = true;
//red is false and blue is true
mirror = false;
_driveReset(); // reset the drive encoders
reset_motors();

Task drive_task(driveTask);
Task turn_task(turnTask);
Task tray_task(trayTask);
Task lift_task(liftTask);

Eight_Point_Auton();

drive_task.remove();
turn_task.remove();
tray_task.remove();
lift_task.remove();

}
void drivecontrol(){
  Chasis_L1.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
Chasis_L2.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
Chasis_R1.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
Chasis_R2.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
Tray.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);

Chasis_L1.move(master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y));
Chasis_L2.move(master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y));
Chasis_R1.move(master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y));
Chasis_R2.move(master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y));
}

//*********************************DRIVER MODE
void opcontrol() {

while (true) {
drivecontrol();
intakemove();
liftmov();
traymove();

}
}
