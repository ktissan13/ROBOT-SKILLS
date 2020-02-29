#include "main.h"

void reset_motors(){
  lift_reset();
  intake_reset();
  tray_reset();
  _driveReset();
}

void deploy(int distance, int vel){
  reset_motors();
  trayAsync(240, 100);
  move_lift(distance,vel);
  move_lift(-1*distance, 200);
  trayAsync(-240, 200);
}
