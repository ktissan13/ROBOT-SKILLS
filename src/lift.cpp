#include "main.h"

Motor Lift(2,MOTOR_GEARSET_36,true, MOTOR_ENCODER_DEGREES);

static int liftTarget = 0;
static int lift_distance = 0;
static int lift_speed = 0;

void lift_reset(){
  Lift.tare_position();
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
