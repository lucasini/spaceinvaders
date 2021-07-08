#include "control.h"

/** Semafori      */
sem_t freee;      //  posizioni vuoto
sem_t occupati;   //  posizioni occupate
sem_t semaforo;   //  sincronizzatore per gli accessi al buffer

/** Indici buffer */
int insertt = 0;  //  indice per l'inserimento
int removee = 0;  //  indice per la rimozione


Object buffer[MAX_OBJECT];  //  buffer di appoggio

/**
 * Inizializza lo schermo
 */
void init_preliminary(){
    initscr();

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    
    noecho();
    curs_set(0);
}

/**
 * Inizializza tutti i semafori
 */
void init_semaphore(){
  sem_init(&semaforo, 0, 1);        //  inizializzo semaforo per sincronizzare accessi al buffer
  sem_init(&occupati, 0, 0);        //  inizializzo semaforo per impedire l'accesso di un thread al buffer
  sem_init(&freee, 0, DIM_BUFFER);  //  inizializzo semaforo per le posizioni libere all'interno del buffer
}

/**
 * Distrugge tutti i semafori inizializzati in precedenza
 */
void destroyer_semaphore(){
	sem_destroy(&semaforo);
	sem_destroy(&freee);
	sem_destroy(&occupati);
}

/**
 * Ogni elemento risiede in buffer generale nel quale si trovano tutti gli elementi aventi thread distinti.
 * La funzione add_buffer, si occupa di aggiungere al buffer l'oggetto passato come parametro.
 */
void add_buffer(Object object){
  sem_wait(&freee);     //  semaforo posizioni libere in attesa
  sem_wait(&semaforo);  //  semaforo per sincronizzazione in attesa

  buffer[insertt] = object;             //  Inserisco l'oggetto nel buffer
  insertt = (insertt + 1) % MAX_OBJECT;

  sem_post(&semaforo);  //  incrementa valore semaforo
  sem_post(&occupati);  //  incrementa valore semaforo
}

/**
 * Rimuove l'elemento passato come parametro dal buffer
 */
int remove_buffer(Object *object){
  sem_wait(&occupati);  //  
  sem_wait(&semaforo);

  *object = buffer[removee];    //  Recupero un oggetto dal buffer
  removee = (removee + 1) % MAX_OBJECT;

  sem_post(&semaforo);
  sem_post(&freee);

  return 1;
}

/**
 * Restituisce una posizione libera all'interno del buffer
 */
int free_position_object(){
  int i;

  for(i = 1; i<MAX_OBJECT;i++){
    if(max_object[i].position == -1){
      max_object[i].position = i;
      return i;
    }
  }

  return -1;
}

/**
 * Usata per le stampe finali
 */
void stampa(int life){
  int i  = 0;

  if(life != 0) {
    for (i = 0; i <5; i++) {
      printf("%s\n", win[i]);
    }
  }
  else{
    for (i = 0; i <5; i++) {
      printf("%s\n", gameover[i]);
    }
  }
}

/**
 * Stampa oggetto
 */
void print_obj(int x, int y, const char *ob[], int obj_size){
  int i = 0;

  for(i = 0; i < obj_size; i++){
    mvaddstr(y+i, x, ob[i]);
  }
}

/**
 * Cancella oggettp
 */
void clear_obj(int x, int y, int obj_size){
  int i,j;

  for(i = 0; i < obj_size; i++){
    for(j = 0; j < obj_size; j++){
      mvaddch(y+i, x+j, ' ');
    }
  }
}

/**
 * collisionCheck -> gestisce le collisioni tra due oggetti 
 * se due oggetti sono in collisione restituisce true
 * altrimenti false
*/
_Bool collisionCheck(Object *object_1, Object *object_current){

  if( ((object_current->new.x >= object_1->new.x) && (object_current->new.x <= (object_1->new.x + object_1->size_object))) ||
      ((object_current->new.x < object_1->new.x) && ((object_current->new.x + object_current->size_object) > object_1->new.x))){
      if( ((object_current->new.y >= object_1->new.y) && (object_current->new.y < (object_1->new.y + object_1->size_object))) ||
          ((object_current->new.y < object_1->new.y) && ((object_current->new.y + object_current->size_object) > object_1->new.y))) { return true; }
    }
  return false;
}

/**
 * get_type_collision -> si occupa di restituire il tipo di collisione sull'oggetto interessato
 */
int get_type_collision(int position){

  if(position > MAX_OBJECT){ return ID_NULL; }

  pthread_mutex_lock(&max_object[position].collision_mutex);  //  blocco mutex

  int collision_type = max_object[position].collision_type;   //  ricavo tipo di collisione
  max_object[position].collision_type = ID_NULL;              //  resetto stato collisione oggetto
  
  pthread_mutex_unlock(&max_object[position].collision_mutex);//  sblocco mutex
  
  //  restituisco il valore associato alla collisione
  return collision_type;
}

/**
 * set_collision_object -> si occupa di settare il tipo di collisione sul oggetto passato come parametro
 */
void set_collision_object(Object *obj, int type_collision_obj){

  pthread_mutex_lock(&obj->collision_mutex);  //  blocco mutex oggetto
  
  obj->collision_type = type_collision_obj;   //  assegno collisione a oggetto
  
  pthread_mutex_unlock(&obj->collision_mutex);//  sblocco il mutex

}


/**
 * game_area si tratta del cuore del programma, al suo interno vengono gestite le
 * collisioni tra gli oggetti presenti a schermo, ogni singolo oggetto presente nel
 * buffer viene letto all'interno di questa funzione e comunica con i vari task tramite 
 * appositi controlli, restituisce il numero di vite della navicella del giocatore per
 * poi effettuare delle stampe finali.
 */
int game_area(){

  Object *object_current,   //  Oggetto corrente 
          rv_data;          //  Variabile d'appoggio

  int id_corrente,            //  id oggetto corrente
      i,                      //  indice 
      number_collision = 0;   //  contatore per le collisioni
  int enemy_number = N_ENEMY; //  Numero di nemici usata per stampare a schermo 

  int coll = 0,   //  coll utilizzata per verificare l'avvenuta collisione 
      life = 1;   //  life utilizzata per l'uscita dal ciclo


  //  inizializzazione ncurses
  init_preliminary();

  do{
    refresh();

    //  Recupero il primo elemento dal buffer
    if(!remove_buffer(&rv_data)){ continue; }

    //  Salvo l'elemento recuperato dal buffer e lo leggo in area di gioco
    object_current = &max_object[rv_data.position];
    id_corrente = rv_data.position;

    number_collision = 0; //  reset contatore collisioni

    //  Gestione delle collisioni, controllo che current sia presente in area di gioco
    if(object_current->position != -1){
     
      for(i = 0; i < 50; i++){

        //  collisione con se stesso
        if(id_corrente == i){ continue; }

        //  collisione con oggetto non presente in area di gioco
        if(max_object[i].position == -1){ continue; }

        //  Se una bomba colpisce un alieno, nessun problema
				if( ((max_object[i].ID == ID_BOMB) && (object_current->ID == ID_ENEMY)) ||
					  ((max_object[i].ID == ID_ENEMY) && (object_current->ID == ID_BOMB))){ continue; }

				//  Se due missili si colpiscono, nessun problema
				if((max_object[i].ID == ID_BULLET) && (object_current->ID == ID_BULLET)){ continue; }

        //  verifica state_collision dell'oggetto corrente e quello nel buffer
        coll = collisionCheck(&max_object[i], object_current);

        //  Verifico lo stato di coll
        if(coll){
          number_collision = 1; //  c'è stata una collisione

          //  Verifico che tipo di oggetto è current
          switch(object_current->ID){
            case ID_BULLET: //  missile

              if(max_object[i].ID == ID_ENEMY){     //  collisione missile/enemy
                set_collision_object(&max_object[i], ID_BULLET);     
              }
              else if(max_object[i].ID == ID_BOMB){ //  collisione missile/bomba
                set_collision_object(object_current, REMOVE_ENTITY); 
              }
              set_collision_object(object_current, REMOVE_ENTITY);
              break;
            case ID_BOMB:   //  bomba

              if(max_object[i].ID == ID_SHIP){      //  collisione bomba/spaceship
                set_collision_object(&max_object[i], REMOVE_ENTITY);
                set_collision_object(object_current, REMOVE_ENTITY);
                 
                life = 0;//Esco dal ciclo
                usleep(1000);
              }
              set_collision_object(object_current, REMOVE_ENTITY);  //  Rimuovo la bomba
              break;
            case ID_ENEMY:  //  enemy  
           
              if(max_object[i].ID == ID_SHIP){ life = 0; } // Esco dal ciclo
              
              if(max_object[i].ID == ID_BULLET){
                set_collision_object(object_current, REMOVE_ENTITY);
                set_collision_object(&max_object[i], REMOVE_ENTITY);
              }
              break;
            case ID_SHIP: //  spaceship
              if(max_object[i].ID == ID_ENEMY){ life = 0; } //  Esco dal ciclo
              break;
          }
          refresh();
        }
        
        //  Se il nemico scende alla y della spaceship esco dal ciclo
        if((object_current->ID == ID_ENEMY)&&(object_current->new.y == MAXY-(SHIP_Y+2))){ life = 0; }
      }
    }

    //  Cancellazione di tutti gli elementi
    if(object_current->position != -1){ clear_obj(object_current->new.x, object_current->new.y, object_current->size_object); }

    *object_current = rv_data;

    //Stampa di tutti gli elementi
    switch(rv_data.ID) {

      case ID_ENEMY:

        if(object_current->life == -1){
          enemy_number--;//Decremento il numero di nemici utilizzato per l'uscita dal buffer
          if(enemy_number == 0){  //  Se ho ucciso tutti gli alieni segnalo l'eliminazione della space_ship
            set_collision_object(&max_object[0], REMOVE_ENTITY); 
          }
          continue;
        }
        switch (object_current->level) {//In base al livello stampo il nemico
          case LEVEL1:
            print_obj(object_current->new.x, object_current->new.y, enemyl1, 3);
            break;
          case LEVEL2:
            print_obj(object_current->new.x, object_current->new.y, enemyl2, 3);
            break;
        }
        break;//Stampo tutti gli oggetti letti
      case ID_SHIP:

        print_obj(object_current->new.x, object_current->new.y, spaceship, 5);
        break;
      case ID_BOMB:

        print_obj(object_current->new.x, object_current->new.y, bomb_enemy_, 1);
        break;
      case ID_BULLET:

        print_obj(object_current->new.x, object_current->new.y, bullet, 1);
	      break;
      case REMOVE_ENTITY: //Se un'oggetto è da rimuovere lo rimuovo e attendo la terminazionedel suo thread
       
        object_current->position = -1;
        pthread_join(object_current->thread, NULL); //  attendo la terminazione di tutti i thread 
        break;
    }
    
    curs_set(0);
    refresh();
    mvprintw(0,0,"Enemy: %d", enemy_number);
    refresh();

  }while((life != 0 ) && (enemy_number > 0));
  
  attroff(COLOR_PAIR(1)); //  Termino i colori
  endwin();               //  Termino la ncurses
  
  //Se son rimasti thread attivi nel buffer li elimino mandando un segnale
  for(i = 0; i < MAX_OBJECT; i++){
    if(max_object[i].position != -1){
      set_collision_object(&max_object[i], REMOVE_ENTITY);
      max_object[i].position = -1;
    }
  }
  usleep(1000000);
  return life;
}
