#include "main.h"

//*********************************SKILLS CODE
//*********************************ENABLE SKILLS IN AUTNOMOUS 
void super_sayin(){

  drive(-0.5 TL);
  turn(140);
  turn(-1);
  drive(1.5 TL);
  intake(200);
  delay(20);
  intake(0);
  drive(-0.3 TL);
  move_tray(200, 200);
  move_lift(500, 200);
  drive(0.4 TL);
  move_intake(400, 200);

}
