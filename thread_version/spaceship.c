#include "spaceship.h"
/*
spaceship_task si occupa del movimento della navicella madre, la navicella si
muove tramite la pressione dei tasti freccia verso destra e sinistra, alla
pressione della barra spaziatrice, viene chimato il task per il bombardamento
bullet_task, i due missili che si muovono verso gli angoli
superiori della finestra dello schermo
*/
void *spaceship_task(void *arg){

  Object my_spaceship;
  char input_player;//Input player movimenti e spari

  int collision_type;
  int life = 1;
  int index;

  my_spaceship = *((Object *)(arg));

  my_spaceship.new.x = MAXX/2;
  my_spaceship.new.y = MAXY-(SHIP_Y+2);
  my_spaceship.life = 1;

  add_buffer(my_spaceship);

  while(life){
    //  Lettura collisioni
    collision_type = get_type_collision(my_spaceship.position);
    //  se la collisione ricevuta è REMOVE_ENTITY imposto la vita a 0
    if(collision_type == REMOVE_ENTITY){
      life = 0;
      continue;
    }

    //  Movimento
    switch(input_player = getch()){
      case SX:
        if(my_spaceship.new.x > 0){ my_spaceship.new.x -= PASSO; }
      break;
      case DX:
        if(my_spaceship.new.x < (MAXX-1)){ my_spaceship.new.x += PASSO; }
        break;
      //Spari
      case SPACE_BAR:
        //  Cerco una posizione libera nel buffer e lancio il thread proiettile, posizionandoli all'altezza della spaceship
        //  Missile sinistro
        index = free_position_object();
        max_object[index].direction = SINISTRA;
        max_object[index].new.x = my_spaceship.new.x + my_spaceship.size_object - 3;
        max_object[index].new.y = my_spaceship.new.y - 1;
        max_object[index].size_object = BULLET_X;
        pthread_create(&max_object[index].thread, NULL, &bullet_task, &max_object[index]);
        // Missile destro
        index = free_position_object();
        max_object[index].direction = DESTRA;
        max_object[index].new.x = my_spaceship.new.x + my_spaceship.size_object - 3;
        max_object[index].new.y = my_spaceship.new.y - 1;
        max_object[index].size_object = BULLET_X;
        pthread_create(&max_object[index].thread, NULL, &bullet_task, &max_object[index]);
      }

      add_buffer(my_spaceship);
    }

    //sono uscito dal ciclo e segnalo la rimozione dell'oggetto ed elimino il thread
    my_spaceship.ID = REMOVE_ENTITY;
   
    add_buffer(my_spaceship);
    pthread_exit(NULL);
}

/*
bulletR_task si occupa del proiettile destro sparato dalla spaceship
*/
void *bullet_task(void *arg){
  Object bullet;
  int life = 1, 
      coll;//Vita del proiettile e variabile usata per recuperare il tipo di collisione

  bullet = *((Object *) (arg));
  bullet.ID = ID_BULLET;
  bullet.size_object = BULLET_X;
  

  add_buffer(bullet);

  while(life){
    //Verifico le collisioni
    coll = get_type_collision(bullet.position);
    
    //In caso sia richiesta la rimozione esco dal buffer
    if(coll == REMOVE_ENTITY){
      bullet.new.y = -1;
      bullet.new.x = -1;
      life = 0;
    }

    bullet.new.y -=1;
    //Gestisco il movimento verso destra e verso sinistra dei due proiettili
    switch (bullet.direction){
      case SINISTRA:
        bullet.new.x -=1;
        break;
      case DESTRA:
        bullet.new.x +=1;
        break;
    }

    add_buffer(bullet);
    usleep(BULLET_DELAY);
  }
  //Rimuovo l'oggetto e termino il thread
  bullet.ID = REMOVE_ENTITY;
  add_buffer(bullet);
  
  pthread_exit(NULL);
}