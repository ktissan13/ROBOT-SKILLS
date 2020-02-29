#ifndef _TRAY_H_
#define _TRAY_H_

void move_tray(int distance, int vel);
void trayAsync(int distance, int vel);
void trayTask(void* parameter);
void tray(int distance, int vel);
void traymove();
void tray_reset();

#endif
