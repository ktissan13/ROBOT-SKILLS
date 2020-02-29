#ifndef _INTAKE_H_
#define _ITNAKE_H_

void intake(int vel);
void move_intake(int distance, int vel);
void intakeTask(void* parameter);
void intakeAsync(int distance, int vel);
void intakemove();
void intake_reset();

#endif
