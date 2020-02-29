#ifndef _LIFT_H_
#define _LIFT_H_

void move_lift(int distance, int vel);
void liftAsync(int distance, int vel);
void liftTask(void* parameter);
void lift(int vel);
void liftmov();
void lift_reset();

#endif
