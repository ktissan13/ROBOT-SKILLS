#include "main.h"

void reset_motors(){
  lift_reset();
  intake_reset();
  tray_reset();
  _driveReset();
}

void deploy(int distance, int vel){
  reset_motors();
  move_tray(240, 200);
  move_lift(distance,vel);
  move_lift(-1*(distance-5), 200);
  trayAsync(-240, 200);
}
