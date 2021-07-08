#ifndef SPACE_INVADERS_H
#define SPACE_INVADERS_H


/*Librerie*/
#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>


/*============================================*/
/*Macro*/
#define PASSO 1       //  Entità spostamento
#define SU 65         //  Freccia su
#define GIU 66        //  Freccia giu
#define SX 68         //  Freccia sx
#define DX 67         //  Freccia dx
#define MAXX 80       //  Numero di colonne schermo
#define MAXY 24       //  Numero di riche schermo
#define SPACE_BAR 32  //  Tasto SpaceBar
/*============================================*/


#define ENEMY_DELEY 60000     //  Ritardo nemici
#define BOMB_DELEY 60000      //  Ritardo bombe
#define BULLET_DELAY 90000    //  Ritardo proiettili
#define BOMB_TIME 100         //  Iterazioni bombe

#define DIM_BUFFER 100  //  Dimensione del buffer d'appoggio
#define MAX_OBJECT 200  //  Dimensione del buffer principale

#define N_ENEMY 6 //  Numero di nemici presenti nella scena

/*  Identificatori oggetti  */
#define ID_SHIP 0         //Identificatore spaceship
#define ID_BULLET 1       //Identificatore proiettile
#define ID_ENEMY 3        //Identificatore nemico
#define ID_BOMB 4         //Identificatore bomba
#define ID_NULL 5         //Identificatore nullo
#define REMOVE_ENTITY 6   //Identificatore eliminazione entità

/*  macro livelli */
#define LEVEL1 1    //  livello1 enemy
#define LEVEL2 2    //  livello2 enemy


typedef enum {SINISTRA, DESTRA} dir;  //  Enumerazione direzione oggetti 


/*  Struct Posizione  */
typedef struct {
	int x;    // coordinata x
	int y;    // coordinata y
}Posizione;


/*  Object struct */
typedef struct{
  int level;                        //  Livello 
  int position;                     //  Posizione nel buffer
  int ID;                           //  Identificatore
  int hit;                          //  Colpi ricevuti
	int life;                         //  Vita
	int size_object;                  //  Dimensione
	int collision_type;               //  Tipo di collisione
  dir direction;                    //  Direzione 
  Posizione new;                    //  Posizione 
  pthread_t thread;                 //  Thread 
	pthread_mutex_t collision_mutex;  //  Mutex per collisioni
}Object;


Object max_object[MAX_OBJECT];      //  Buffer principale


/**
 * Stampe finali
 */
static char gameover[5][61] = {
"  ____                          ___                       ",
" / ___| __ _ _ __ ___   ___    / _ \\__   _____ _ __      ",
"| |  _ / _` | '_ ` _ \\ / _ \\  | | | \\ \\ / / _ \\ '__| ",
"| |_| | (_| | | | | | |  __/  | |_| |\\ V /  __/ |        ",
"\\____|\\__,__|_| |_| |_|\\___|   \\___/  \\_/ \\___|_|   "};

static char win[5][47] = {
"  __        ___                            ",
"  \\ \\      / (_)_ __  _ __   ___ _ __      ",
"   \\ \\ /\\ / /| | '_ \\| '_ \\ / _ \\ '__| ",
"    \\ V  V / | | | | | | | |  __/ |         ",
"     \\_/\\_/  |_|_| |_|_| |_|\\___|_|       "};

/**
 * Sprite
 */
static const char *spaceship[] = {
    "     ",
    " .^. ",
    "<._.>",
    " !!! ",
    "     "
};
static const char *enemyl1[] = {
  "vUv",
  " V ",
  "   "
};
static const char *enemyl2[] = {
  "T T",
  "   ",
  "T T"
};
static const char *bomb_enemy_[] = {"o"};
static const char *bullet[] = {"*"};


#include "control.h"
#include "spaceship.h"
#include "enemy.h"


#endif
