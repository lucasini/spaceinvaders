#ifndef SPACESHIP_H
#define SPACESHIP_H


#include "spaceinvaders.h"

/// --------------------------------------------------- SPACESHIP
void spaceship_task(int pipe_out);
void space_ship_end_handler(int signum);
/// --------------------------------------------------- MISSILE
void missile_task(int pipe_out, Direction dir, int x, int y);
void missile_end_handler(int signum);


#endif