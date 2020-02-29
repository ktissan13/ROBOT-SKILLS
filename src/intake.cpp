#include "main.h"

//*********************************MOTOR SETUP
Motor Intake_R(1,MOTOR_GEARSET_18,true, MOTOR_ENCODER_DEGREES);
Motor Intake_L(12,MOTOR_GEARSET_18,false, MOTOR_ENCODER_DEGREES);

//*********************************STATIC VARIABLES
static int intakeTarget = 0;
static int intake_distance = 0;
static int intake_speed = 0;

//*********************************AUTONMOUSE FUNCTIONS
void intake(int vel){
  Intake_L.move_velocity(vel);
  Intake_R.move_velocity(vel);
}

void intake_reset(){
  Intake_L.tare_position();
  Intake_R.tare_position();
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

//*********************************AUTON TASK
void intakeTask(void* parameter){
  while(true){
    if (auton_mode){
      delay(20);

      switch(intakeTarget){
        case 1:
        move_intake(intake_distance, intake_speed);
        break;
      }
      intakeTarget = 0;
    }
  }
}

void intakeAsync(int distance, int vel){
  intake_distance = distance;
  intake_speed = vel;
  intakeTarget = 1;
}

//*********************************DRIVER TASK
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
