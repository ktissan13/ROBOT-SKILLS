#include "main.h"
#define MAX 200;

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

   setSpeed(60);
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
   drive(distance);
   setSlant(s);
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

   while(auton_mode){
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

 void turnTask(void* parameter){
   int prevError;

   while(auton_mode){
   pros::delay(20);

     if(driveMode != 0)
       continue;

     int sp = turnTarget;

     if(sp > 0)
       sp *= 2.35;
     else
       sp *= 2.3;

     double kp = .9;
     double kd = 3.5;

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

void move_lift(int distance, int vel){
  Lift.tare_position();
  while(Lift.get_position() != distance){
    Lift.move_absolute(distance, vel);
    if (Lift.get_position() >= (distance-5) && Lift.get_position() <= (distance+5)){
      break;
}}}

void non_slew_drive(int distance, int vel){
  _driveReset();
  while(Chasis_R1.get_position() != distance){
    Chasis_left(vel);
    Chasis_right(vel);
    if(Chasis_R1.get_position() >= (distance-5) && Chasis_R1.get_position() <= (distance+5)){
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
  deploy(600,60);
  // Intake 4 cubes
  intake(600);
  drive(2.5 TL);
  // Cube in the middle of the pole
  turn(-45);
  drive(0.5 TL);
  // Stack in the platform
  drive(-.5 TL);
  turn(90);
  drive(3 TL);
  move_intake(-1000, 600);
  move_tray(800,600);
}

void disabled() {}
void competition_initialize() {}

//*********************************AUTONMOUS MODE
void autonomous() {
auton_mode = true;
mirror = false;
_driveReset(); // reset the drive encoders
reset_motors();
Task drive_task(driveTask);
Task turn_task(turnTask);

super_sayin();

drive_task.remove();
turn_task.remove();
}

//*********************************DRIVER MODE
void opcontrol() {}
