#include "main.h"

Controller master(E_CONTROLLER_MASTER);
Controller partner(E_CONTROLLER_PARTNER);

//*********************************DRIVER MODE
void opcontrol() {

while (true) {
auton_mode = false;
drivecontrol();
intakemove();
liftmov();
traymove();
}
}
