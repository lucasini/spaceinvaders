#include "spaceinvaders.h"


int main() {
  
  /*
    Variabili d'appoggio, indice per scorrere il buffer, posizione iniziale (x,y),
    variabile per il salvataggio delle vite della spaceship
  */
  int i = 0, 
      startx = 0, 
      starty = 2, 
      life; 

  //  Inizializzo l'accesso al buffer
  init_semaphore();

  //  Inizializzazione preliminare del buffer
  for(i = 0; i< MAX_OBJECT; i++){ max_object[i].position = -1; }

  //  Creazione della navicella madre
  max_object[0].ID = ID_SHIP;           //  Identificatore oggetto 
  max_object[0].position = 0;           //  Posizione spaceship nel buffer
  max_object[0].size_object = SHIP_X;   //  Dimensione 
  pthread_create(&max_object[0].thread, NULL, &spaceship_task, &max_object[0]); //  Creazione del thread
  
  //  Creazione degli enemy
  for(i = 1; i < N_ENEMY+1; i++){

    startx = (5)*i+1*2;                   //  Aggiorno la coordinata x a ogni ciclo del for
    max_object[i].ID = ID_ENEMY;          //  Identificatore oggetto
    max_object[i].position = i;           //  Posizione dell'enemy i nel buffer
    max_object[i].size_object = ENEMY_X;  //  Dimensione
    max_object[i].life = 4;               //  Vite 
    max_object[i].hit = 0;                //  Numero di colpi ricevuti
    max_object[i].collision_type = 7;     //  Tipo di collisione (7 collisione con missile)
   
    //  
    if(i%2 == 1){
      max_object[i].new.x = (MAXX-startx);    //  Coordinata x
      max_object[i].new.y = starty;           //  Coordinata y
      max_object[i].direction = SINISTRA;     //  Direzione
      max_object[i].level = LEVEL1;           //  Livello di partenza 
    }
    else{
      max_object[i].new.x = startx;           //  Coordinata x
      max_object[i].new.y = starty;           //  Coordinata y
      max_object[i].direction = DESTRA;       //  Direzione
      max_object[i].level = LEVEL1;           //  Livello di partenza
      starty += ENEMY_Y;                      //  Aggiornamento coordinata y a ogni generazione di un nemico con indice pari
    }

    //Creo n thread e passo ogni oggetto creato a enemy_task
    pthread_create(&max_object[i].thread, NULL, &enemy_task, &max_object[i]);
  }

  life = game_area();     //  Area di gioco, restituisce le vite della navicella e gestisce l'azione di gioco

  stampa(life);           //  Stampa finale
  destroyer_semaphore();  //  Distruzioe semafori

  return 0;
}
