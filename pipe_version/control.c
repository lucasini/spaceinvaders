#include "control.h"



/// --------------------------------------------------- CONTROLLO
/**
 *  Funzione per l'area di gioco e gestione oggetti
 *  @param pipein Pipe dal quale leggere le info degli oggetti
 */
void control_task(int pipein){
    int obj_n = 1;			// 	Numero di oggetti nella scena

    char buffer[30];		// 	Buffer per la stampa del numero di enemy
    int i;                  //  indice 
    int enemy = M;          //  numero di enemy

    Object read_data;           //  variabile che ospita l'oggetto letto dalla pipe
    Object objects[MAX_BUFF];   //  buffer che contiene gli oggetti nell'area di gioco
    Object *current;            //  oggetto corrente
    int current_id;             //  id oggetto corrente

    _Bool playing = true;       // 	Variabile status di game
    _Bool col = false;          // 	Variabile di avvenuta collisione
    Collision data;             // 	Dato da inviare a enemy su stato collisione


    objects[0].ID = SPACESHIP;                         //  Inizializzo il primo oggetto nell'array come spaceship
    for(i = 0; i < MAX_BUFF; i++) {objects[i].x = -1;} //  inizializzo l'array degli oggetti nell'area di gioco


    do {
        read(pipein, &read_data, sizeof(Object));	// Ricevo i dati di un oggetto dalla pipe
        current = NULL;                             //  inizializzo current a NULL

        //  verifico che l'oggetto letto dalla pipe non sia la spaceship
        if(read_data.ID != SPACESHIP){
            for(i = 1; i < MAX_BUFF; i++){              //  scorro il buffer per cercare l'oggetto letto da pipe
                if(read_data.pid == objects[i].pid){    //  se il pid dell'oggetto in posizione i è uguale al pid del dato letto:
                    current 	= &objects[i];          //  assegno l'oggetto a current
                    current_id 	= i;                    //  salvo l'indice dell'oggetto
                    i 			= MAX_BUFF;             //  imposto i a MAX per uscire dal for
                }
            }
            //  se current non è presente nel buffer degli oggetti in area di gioco
            if(current == NULL){                        
                for(i = 1; i < MAX_BUFF; i++){          //  scorro il buffer
                    if(objects[i].x == -1){             //  alla prima posizione libera che trovo:
                        obj_n++;                        //  incremento numero di oggetti nella scena
                        current     = &objects[i];      //  assegno lo "spazio libero" a current per la successiva aggiunta
                        current_id  = i;                //  salvo l'indice dello spazio libero
                        i 		    = MAX_BUFF;         //  imposto i a MAX per uscire dal for
                    }
                }
            }
        }
        else{	                    //  l'oggetto letto da pipe è la spaceship
            current = &objects[0];  //  assegno a current l'elemento del buffer in cui è presente la spaceship
            current_id = 0;         //  id oggetto corrente
        }


        if(current->x != -1){   //  Se l'oggetto corrente è presente nell'area di gioco, verifico le collisioni

            //  scorro tutti gli oggetti all'interno del buffer
            for(i = 0; i < MAX_BUFF; i++){

                // 	collisione con se stesso
                if(current_id == i) { continue; }

                // 	collisione con oggetto non presente
                if(objects[i].x == -1) { continue; }

                // 	se bomba colpisce enemy, continuo
                if(((objects[i].ID == BOMB) && (current->ID == ENEMY)) ||
                   ((objects[i].ID == ENEMY) && (current->ID == BOMB))) { continue; }

                //  collisione tra due oggetti missile, continuo
                if(((objects[i].ID == MISSILE) && (current->ID == MISSILE))) { continue; }

                // 	verifico collisione tra current e l'oggetto i del buffer
                col = collisionCheck(&objects[i], current);

                //  gestione delle collisioni
                if(col){

                    //  gestisco collisioni per ogni tipo di oggetto
                    switch(current->ID){
                        case MISSILE:       //  collisione oggetto missile
                            if(objects[i].ID == ENEMY){         //  collsione missile/alieno
                                data = COLLISION;           
                                write(objects[i].pipes[1], &data, sizeof(Collision));
                            }
                            else if(objects[i].ID == BOMB){     //  collisione missile/bomba
                                kill(current->pid, SIGQUIT);   
                            }
                            kill(current->pid, SIGQUIT);
                            break;
                        case ENEMY:         //  Collisione oggetto alieno
                            if(objects[i].ID == MISSILE){ 	    // 	collisione enemy/missile
                                data = COLLISION;
                                write(current->pipes[1], &data, sizeof(Collision));

                                kill(objects[i].pid, SIGQUIT);
                            }
                            else if(objects[i].ID == SPACESHIP){	// Se un alieno collide con la navicella, fine gioco
                                playing = false;
                                usleep(SLEEP_GAMEOVER);
                            }
                            break;
                        case BOMB:	        // Collisione oggetto bomba
                            if(objects[i].ID == SPACESHIP){     //  collisione bomba/spaceship
                                playing = false;
                                usleep(SLEEP_GAMEOVER);
                            }
                            else if(objects[i].ID == MISSILE){  // collisione bomba/missile
                                kill(objects[i].pid, SIGQUIT);
                                kill(current->pid, SIGQUIT);
                            }
                            break;
                        case SPACESHIP:	    //  Collisione oggetto spaceship
                            if((objects[i].ID == BOMB) || (objects[i].ID == ENEMY)){        //  collisione spaceship/bomba
                                playing = false;
                                usleep(SLEEP_GAMEOVER);
                            }
                            break;
                    }
                    
                    refresh();  //  aggiorno schermo
                }

                //  se l'oggetto corrente è un enemy e arriva in fondo allo schermo, termino il programma
                if((current->ID == ENEMY) && (current->y == (MAXY - (SPACESHIP_SIZE)))){
                    playing = false;
                    usleep(SLEEP_GAMEOVER);
                }           
            }
        }

        // 	aggiorno posizione di un oggetto presente nell'area di gioco
        if(current->x != -1) { delete(current->x, current->y, current->size); }

        *current = read_data;   //  aggiungo l'oggetto letto dalla pipe all'area di gioco, tramite current

        // 	A seconda dell'oggetto letto dalla pipe, eseguo un operazione differente
        switch(read_data.ID){
            case ENEMY:
                //  Se la vita di un alieno e' negativa, deve essere rimosso dall'area di gioco; 
                if(current->life == -1){
                    close(current->pipes[1]);   //  Chiudo la pipe
                    enemy--;
                    continue;
                }
                //  disegno sprite enemy a seconda del livello
                switch(current->level){
                    case LEVEL1:
                        print(current->x, current->y, SENEMY1, ENEMY_SIZE);
                        break;
                    case LEVEL2:
                        print(current->x, current->y, SENEMY2, ENEMY_SIZE);
                        break;
                }
                break;            
            case SPACESHIP:
                //  disegno sprite spaceship
                print(current->x, current->y, SSPACESHIP, SPACESHIP_SIZE);
                break;
            case BOMB:
                //  disegno sprite bomba
                mvaddch(current->y, current->x, SBOMB);
                break;
            case MISSILE:
                //  disegno sprite missile
                mvaddch(current->y, current->x, SMISSILE);
                break;
            case DELETE:
                //  oggetto che deve essere rimosso dall'area di gioco
                current->x = -1;        //  imposto posizione a -1
                obj_n--;                //  decremento il numero di oggetti presenti nell'area di gioco
                break;
        }

        //  stampo numero di alieni sullo schermo
        mvaddstr(0, 0, "\t\t\t\t\t\t\t\t\t\t");
        sprintf(buffer, "Enemy: %d", enemy);
        mvaddstr(0, 0, buffer);

        //  Rendo invisibile il cursore e faccio il refresh
        curs_set(0);
        refresh();

    } while(playing && enemy > 0);


    // termino tutti i processi
    for (i = 0; i < MAX_BUFF; i++){
        if(objects[i].x != -1){
            // Se e' un alieno utilizzo la pipe delle collioni per inviare il messaggio che riguarda la sua eliminazione
            if(objects[i].ID == ENEMY){
                data = DEATH;
                write(objects[i].pipes[1], &data, 1);
            }
            else{
                kill(objects[i].pid, SIGKILL);
            }
            obj_n--;            //  decremento il numero di oggetti
            objects[i].x = -1;  //  inizilizzo la posizione
        }
    }

    //  salvo il risultato di vittoria (true) o sconfitta (false)
    if(enemy == 0){ result = true; }
    else{ result = false; }
}
/**
 * funzione che controlla la collisione tra un oggetto i e current
 * @param object oggetto i
 * @param current oggetto corrente
 * @return true se vero
 */
_Bool collisionCheck(Object *object, Object *current){
    if(((current->x >= object->x) && (current->x <= (object->x + object->size))) ||
       ((current->x < object->x) && ((current->x + current->size) > object->x))){
        if(((current->y >= object->y) && (current->y < (object->y + object->size))) ||
           ((current->y < object->y) && ((current->y + current->size) > object->y)))
            return true;
    }
    return false;
}


/// --------------------------------------------------- UTILITA
/**
 *  procedura per la stampa di una sprite
 *  @param x coordinata x
 *  @param y coordinata y
 *  @param data array di stringhe
 *  @param size dimensione
 */
void print(int x, int y, const char *data[], int size){
    int i;

    for(i = 0; i < size; i++){
        mvaddstr(y + i, x, data[i]);
    }
}
/**
 *  procedura per cancellare un oggetto, la sua sprite viene sovrascritta da spazi vuoti
 *  @param x coordinata x
 *  @param y coordinata y
 *  @param size dimensione
 */
void delete(int x, int y, int size){
    int i;
    int j;

    for(i = 0; i < size; i++){
        for(j = 0; j < size; j++){
            mvaddch(y+i, x+j, ' ');
        }
    }
}
/** procedura per inizializzare la finestra di gioco    */
void init_scr(){
        initscr();	        // Inizializzo la ncurses
        
        start_color();                          //  avvio i colori
        init_pair(1, COLOR_GREEN, COLOR_BLACK); //  coppia di colori
        attron(COLOR_PAIR(1));                  //  abilito la coppia di colori
        
        noecho();           //  disabilito visibilità caratteri
        srand(time(NULL));  //  inizializzo il generatore random
        curs_set(0);        //  nascondo il cursore
}
/** procedura per terminare la finestra di gioco        */
void end_scr(){
        attroff(COLOR_PAIR(1));
        endwin();
}
/** 
 * procedura stampa WIN o GAMEOVER                     
 * @param status risultato partita
 */
void stampa(_Bool status){
  int i  = 0;

  if(status) {
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
