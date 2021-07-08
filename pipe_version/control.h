#ifndef CONTROL_H
#define CONTROL_H


#include "spaceinvaders.h"

/// --------------------------------------------------- CONTROLLO
void control_task(int pipein);
_Bool collisionCheck(Object *object, Object *current);
/// --------------------------------------------------- UTILITA
void print(int x, int y, const char **data, int size);
void delete(int x, int y, int size);
void init_scr();
void end_scr();
void stampa(_Bool status);


#endif