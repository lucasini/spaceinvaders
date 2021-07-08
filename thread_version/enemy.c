#include "enemy.h"

/*
=======================================================================
Gestione Aliena
=======================================================================
*/
/*
enemy_task è il task relativo al nemico, gestisce il movimento di un singolo
oggetto e richiama al suo interno bomb_task, il task utilizzato per il rilascio
della bomba nemica
*/
void *enemy_task(void *arg){

  Object enemy;
  int timer = 0,   // timer bombe
      index,      //Usato per la ricezione della cella libera nel buffer
      coll = 7,   //Usato per la ricezione delle collisioni
      i,       //Usato per la ricezione della cella libera nel buffer
      collisione = 0; //Contatore per le collisioni

  _Bool flag = false; //flag per l'uscita dal ciclo e la creazione del nuovo thread nemico

  enemy = *((Object *)(arg)); //assegno a enemy l'oggetto passato con *arg

  add_buffer(enemy);//Inserisco l'oggetto nemico nel add_buffer
  /*
    Itero si quando la vita dell'alieno è -1 oppure al 
    cambio di stato del flag esco per far partire un 
    nuovo thread
  */
  while ((enemy.life != -1) && !flag) {

    //Collision controll
    coll = get_type_collision(enemy.position);
    //Collisione enemy proiettili
    if( (coll == ID_BULLET) && collisione >= 2){ 
      coll = 7;
      collisione = 0;
      enemy.life --;
      enemy.hit++;
      if(enemy.level == REMOVE_ENTITY){ //Se l'oggetto corrente è da rimuovere si manda un segnale all'oggetto
        enemy.life = -1;
        break;
      }

      if((enemy.life == 3 && enemy.level == LEVEL1) && enemy.hit == 1 ){ //Aggiornamento delle vite nel caso l'oggetto venisse colpito
        flag = true;
      }else if(enemy.life == 1 && enemy.level == LEVEL2){
        enemy.level = REMOVE_ENTITY;
        enemy.life = -1;
      }

     
    }
    


    if(timer == BOMB_TIME){
     


      index = free_position_object(); //cerco la prima posizione libera nel buffer per metterci il thread bomba

      if(index >= 0){

        //Posiziono la bomba
        max_object[index].new.x = enemy.new.x + enemy.size_object/2;
        max_object[index].new.y = enemy.new.y + enemy.size_object + 1;
        
        pthread_create(&max_object[index].thread, NULL, &bomb_task, &max_object[index]);//Chiamo la bomb task che si occupa di gestire il bombardamento
       
      }
      timer = 0;//Aggiorno il timer
    }

    timer++; //Timer per lo sgancio delle bombe
    collisione++; // Aggiorno il contatore delle collisioni

    // movimento enemy
    switch(enemy.direction){
      case SINISTRA:
        enemy.new.x -= 1;
        if((enemy.new.x == 0) || enemy.new.x == (MAXX/2) + enemy.size_object){
          enemy.direction = DESTRA;
        }

        if(enemy.new.x == 0){
          enemy.new.y += ENEMY_Y;
        }
        
      
        break;
        
        case DESTRA:
          enemy.new.x += 1;
          
          if((enemy.new.x == MAXX - enemy.size_object) || enemy.new.x  == (MAXX/2) - enemy.size_object){
            enemy.direction = SINISTRA;
          }

        if(enemy.new.x == MAXX-enemy.size_object){
          enemy.new.y += ENEMY_Y;
        }
        
          
          break;
          
    }
    

    add_buffer(enemy);//Aggiorno la posizione dell'oggetto nel buffer

    usleep(ENEMY_DELEY);

  }

  //rimuovo il nemico dal buffer è stata richiesta la sua decapitazione
  enemy.ID = REMOVE_ENTITY;

  add_buffer(enemy);

  i = free_position_object(); //CErco una posizione libera nel buffer
  
  /*
    Se il flag ha avuto realmente un cambio di stato e
    l'indice è libero, creo il nemico di livello due, 
    inizializzo i suoi parametri e lancio il nuovo thread
  */
    
    if(flag == true  && i >= 0){

      max_object[i].new.x = enemy.new.x;
      max_object[i].new.y = enemy.new.y;
      max_object[i].ID = ID_ENEMY;  //Identificatore dell'oggetto nemico
      max_object[i].direction = enemy.direction; //Direzione dell'oggetto
      max_object[i].level = LEVEL2; //Livello di partenza
      max_object[i].position = i; //Posizione dell'oggetto i-esimo nel buffer
      max_object[i].size_object = ENEMY_X; //GRandezzaa dell'oggetto
      max_object[i].life = 3; //Vite dell'oggetto nemico
      max_object[i].hit = 0; 
      max_object[i].collision_type = 7;

      pthread_create(&max_object[i].thread, NULL, &enemy_task, &max_object[i]);
    }


  pthread_exit(NULL);//termino il thread

}


/*
bomb_task gestisce il movimento della bomba
*/
void *bomb_task(void *arg){

  Object bombe;

  int coll;

  bombe = *((Object *) (arg));
  
  bombe.ID = ID_BOMB;
	bombe.size_object = 1;
  bombe.life = 1;

  add_buffer(bombe);

   while(bombe.life!=-1){

     coll = get_type_collision(bombe.position);
     //Se la bomba raggiunge la soglia lo faccio uscire dal while uccidendolo
    if((bombe.new.y == MAXY) || (coll == REMOVE_ENTITY)){
      bombe.life = -1;
      
    }
    //Aggiorno la posizione dell'oggetto
    bombe.new.y++;
    add_buffer(bombe);
    usleep(BOMB_DELEY);

  }
  //Esco dal while e segnalo la rimozione dell'oggetto dal buffer ed elimino il buffer
  bombe.ID = REMOVE_ENTITY;

  
  add_buffer(bombe);
  pthread_exit(NULL);
}
