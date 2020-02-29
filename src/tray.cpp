#include "main.h"

//*********************************MOTOR SETUP
Motor Tray(18,MOTOR_GEARSET_36, false, MOTOR_ENCODER_DEGREES);

//*********************************CONSTANT VARIABLES
 static int trayTarget = 0;
 static int tray_distance = 0;
 static int tray_speed = 0;

static int first_position = 270;
static int second_position=400;
static int third_position=700;

//*********************************AUTONMOUS FUNCTIONS
void tray_reset(){
  Tray.tare_position();
}

void move_tray(int distance, int vel){
  Tray.tare_position();
  while(Tray.get_position() != distance){
    Tray.move_absolute(distance, vel);
    if (Tray.get_position() >= (distance-5) && Tray.get_position() <= (distance+5)){
      break;
  }
}
}

//*********************************AUTONMOUS TASK

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

//*********************************DRIVER TASK

void traymove(){
 Tray.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
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
