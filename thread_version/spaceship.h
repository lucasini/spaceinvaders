#ifndef SPACESHIP_H
#define SPACESHIP_H


#include "spaceinvaders.h"


/*Dimensione bullet*/
#define BULLET_X 1
#define BULLET_Y 1

/*Dimensione ship sprite*/
#define SHIP_X 5
#define SHIP_Y 5


/*
spaceship_task si occupa del movimento della navicella madre, la navicella si
muove tramite la pressione dei tasti freccia verso destra e sinistra, alla
pressione della barra spaziatrice, viene chiamato il task per il bombardamento
bullet_task, i due missili si muovono verso gli angoli
superiori della finestra dello schermo
*/
void *spaceship_task(void *arg);

/*
bullet_task si occupa dei due proiettili sparati dalla spaceship
*/
void *bullet_task(void *arg);


#endif