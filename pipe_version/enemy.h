#ifndef ENEMY_H
#define ENEMY_H


#include "spaceinvaders.h"

/// --------------------------------------------------- ALIENO
void enemy_task(int pipeout, int x, int y, int *coll_pipes, Direction d, int level);
/// --------------------------------------------------- BOMBA
void bomb_task(int pipeout, int x, int y);
void bomb_end_handler(int signum);


#endif