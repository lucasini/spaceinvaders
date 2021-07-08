#ifndef ALIEN_H
#define ALIEN_H


#include "spaceinvaders.h"

/*
=======================================================================
Gestione Aliena
=======================================================================
*/

/*Dimensione delle bombe*/
#define BOMB_X 1
#define BOMB_Y 1


/*Dimensione alien sprite*/
#define ENEMY_X 3
#define ENEMY_Y 3


void *enemy_task(void *arg);//task dei nemici
void *bomb_task(void *arg);//Task delle bombe


#endif
