#ifndef CONTROL_H
#define CONTROL_H


#include "spaceinvaders.h"


/*
=======================================================================
Gestione del buffer
=======================================================================
*/
void init_preliminary();            //  Inizializzazione ncurses
void init_semaphore();              //  Inizializzo i semafori ed il buffer
void destroyer_semaphore();         //  Elimino i semafori ed il buffer
void add_buffer(Object object);     //  Aggiungo elementi al buffer
int remove_buffer(Object *object);  //  Rimuovo elementi dal buffer
int free_position_object();         //  Restituisce la posizione libera al'interno del buffer

/*
=======================================================================
Gestione del grafica
=======================================================================
*/
void clear_obj(int x, int y, int obj_size);                     //  Cancella l'oggetto
void print_obj(int x, int y, const char *ob[], int obj_size);   //  Stampa l'oggetto a schermo
void stampa(int life);                                          //  Stampa scritte finale 

/*
=======================================================================
Gestione Collisioni
=======================================================================
*/
_Bool collisionCheck(Object *object_1, Object *object_current); //  Verifica la collisione tra due oggetti
int get_type_collision(int position);                           //  restituisce il tipo di collisione di un oggetto
void set_collision_object(Object *obj, int type_obj);           //  Setta la collisione su un oggetto
int game_area();                                                //  Area di gioco


#endif
