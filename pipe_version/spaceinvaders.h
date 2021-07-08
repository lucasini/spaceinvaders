#ifndef SPACEINVADERS_H
#define SPACEINVADERS_H


#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>


/**     Macro tempi sleep               */
#define SLEEP_UTIME             70000   //     iterazioni  
#define SLEEP_BOMB	        60000   //     iterazioni delle bombe aliene 	
#define SLEEP_GAMEOVER		100000  //     perdita ed il messaggio di gameover 	
#define BOMB_TIME	        100     //     lancio di una bomba (iterazioni) 	

/**     Dimensioni finestra di gioco    */
#define MAXX	78      //      80
#define MAXY	24      //      24

/**     Macro keypad spaceship          */
#define SPACEBAR        32		//    Space Bar per lo sparo
#define SX              68 		//    Freccia sinsitra
#define DX	        67 		//    Freccia destra

/**     Macro oggetti                   */
#define MAX_BUFF        200     // 	Dimensione buffer oggetti in control task
#define M               6       // 	Enemy


/**     Enumerazioni                    */
typedef enum { SPACESHIP, ENEMY, BOMB, MISSILE, DELETE } Type;  // 	Oggetti
typedef enum { LEFT, RIGHT } Direction;                         // 	Direzione
typedef enum { COLLISION, DEATH } Collision;              // 	Collisioni


/**     Struttura oggetto               */      
typedef struct{
	int 	        x;	        // 	Posizione x dell'oggetto
	int	        y;	        // 	Posizione y dell'oggetto
	int 	        size;		// 	Dimensione dell'oggetto 
	int	        life;		// 	Vita dell'oggetto
        int             level;          // 	livello oggetto (utilizzato per gli enemy)
	int	        pipes[2];       // 	Pipe collisioni (utilizzato per gli enemy)
	pid_t	        pid;	        // 	Pid dell'oggetto
	Type	        ID;	        // 	Tipo di oggetto
	Direction       dir;	        // 	Direzione oggetto
} Object;

/**     Dimensioni oggetti              */
#define SPACESHIP_SIZE	5       //	Dimensioni spaceship
#define ENEMY_SIZE     	3	//	Dimensioni enemy
/**     Livelli enemy                   */
#define LEVEL1  1               // 	Livello enemy1
#define LEVEL2  2               // 	Livello enemy2


// 	Sprite enemy livello1
static const char *SENEMY1[] = {
        "vUv",
        " V ",
        "   "
};
//      Sprite enemy livello2
static const char *SENEMY2[] = {
        "T T",
        "   ",
        "T T",
};
//    Sprite spaceship
static const char *SSPACESHIP[] = {
        "     ",
        " .^. ",
        "<._.>",
        " !!! ",
        "     "
};
//    Sprite bomba
#define SBOMB 	        'o'
//    Sprite missile
#define SMISSILE 	'*'


_Bool result;   //      variabile globale sul risultato partita

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



/**     Libreire                        */
#include "enemy.h"
#include "spaceship.h"
#include "control.h"


#endif
