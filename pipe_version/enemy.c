#include "enemy.h"

/// --------------------------------------------------- ALIENO
/**
 * Funzione di gestione di una navicella aliena
 * @param pipeout pipe che scrive nell'area di gioco
 * @param x posizione iniziale x
 * @param y posizione iniziale y
 * @param coll_pipes pipe per le collisioni
 * @param d direzione iniziale oggetto
 * @param level livello della navicella
 */
void enemy_task(int pipeout, int x, int y, int *coll_pipes, Direction d, int level){
    Object alien;       //  oggetto enemy
    Collision data;     //  dato letto dalla pipe delle collisioni
    pid_t new_alien;    //  pid nuovo processo alieno di secondo livello

    //  contatori
    int n = 0;          // 	contatore iterazioni per lo sparo delle bombe
    int coll = 0;   // 	Iterazioni trascorse dall'ultima collisione. E' necessaria per fare in modo che uno stesso missile non colpisca due volte lo stesso oggetto

    //  inizializzazione oggetto enemy
    alien.ID        = ENEMY;
    alien.x         = x;
    alien.y         = y;
    alien.size      = ENEMY_SIZE;
    alien.pid       = getpid();
    alien.pipes[0]  = coll_pipes[0];
    alien.pipes[1]  = coll_pipes[1];
    alien.dir       = d;

    //  in base al livello passato come parametro imposto livello e vita alieno
    switch (level){
        case LEVEL1:
            alien.level = LEVEL1;
            alien.life  = 1;
            break;
        case LEVEL2:
            alien.level = LEVEL2;
            alien.life  = 2;
            break;    
    }
    
    // 	scrive in pipe la posizione iniziale
    write(pipeout, &alien, sizeof(Object));

    //  ciclo finchè le vite di un alieno ha ancora vita
    while(alien.life > 0){
        
        //  se leggo qualche dato dalla pipe delle collisioni
        if(read(alien.pipes[0], &data, sizeof(Collision)) == sizeof(Collision)){
 
            if((data == COLLISION)  && (coll>=2)){   //  Collisione con un missile, decremento le vite
                coll = 0;       //  reinizializzo contatore delle collisioni
                alien.life--;   //  decremento vite 

                //  se un alieno ha terminato le vite ed è di secondo livello, imposto la vita a -1 per poi terminarne il processo
                if(alien.life == 0 && alien.level == LEVEL2){
                    alien.life = -1;
                }
            }
            else if(data == DEATH){	//  imposto le vite a -1 per poi terminare il processo
                alien.life = -1;
                break;
            }
        }

        //  verifico che il contatore n sia uguale a BOMB_TIME perchè un alieno possa sparare una bomba
        if(n == BOMB_TIME){
            //  nuovo processo per la bomba
            pid_t bomb = fork();
            switch(bomb){
                case -1:
                    perror("Impossibile creare il processo Bomba.");
                    exit(-1);
                    
                case 0:
                    bomb_task(pipeout, alien.x + alien.size / 2, alien.y + alien.size + 1);
                    
            }
            n = 0;  //  reinizializzo contatore per il rilascio delle bombe 
        }

        n++;	//  incremento iterazioni per sparo
        coll++;	// 	Aumento il numero di iterazioni trascorse dall'ultima collisione

        //  movimenti alieno
        switch(alien.dir){
            case LEFT:
                alien.x -= 1;
                if((alien.x == 0) || (alien.x == (MAXX/2) + alien.size)){ alien.dir = RIGHT; }
                if(alien.x == 0){ alien.y += alien.size; }
                break;
            case RIGHT:
                alien.x += 1;
                if(alien.x == (MAXX - alien.size) || (alien.x == (MAXX/2) - alien.size )){ alien.dir = LEFT; }
                if(alien.x == MAXX - alien.size){ alien.y += alien.size; }
                break;
        }
        
        //  scrivo aggiornamenti oggetto in pipe
        write(pipeout, &alien, sizeof(Object));
        usleep(SLEEP_UTIME);
    }

    //  l'alieno attuale deve essere cancellato dall'area di gioco
    alien.level++;      //  incremento il livello che passo come parametro per la generazione del nuovo alieno
    alien.life = -1;    //  imposto la vita a -1 per termiare poi il processo
    alien.ID = DELETE;  //  imposto l'ID a DELETE 
    
    write(pipeout, &alien, sizeof(Object)); // 	invio aggiornamento all'area di gioco
    
    //  se il livello dell'alieno attuale è inferiore a 2 creo un nuovo processo
    if(alien.level <= LEVEL2){
        new_alien = fork();

        switch(new_alien){
		    case -1:
			    perror("Impossibile creare il processo Space Ship.");
			    exit(-1);	

		    case 0:
			    enemy_task(pipeout, alien.x, alien.y, alien.pipes, alien.dir, alien.level);
			    
	    }
    }
                
    close(pipeout);         //  chiudo la pipe dalla modalita scrittura in modo che il processo non possa più scrivere in pipe
    close(alien.pipes[0]);  //  chiudo la pipe delle collisioni in modo che l'oggetto non posso più leggere
    exit(0);
}


/// --------------------------------------------------- BOMBA
// Variabile globale per la vita delle bombe
int b_alive = 1;
/** 
 * Handler della bomba per il segnale SIGQUIT 
 * @param signum segnale 
 */
void bomb_handler(int signum){
    //  se il segnale che arriva al processo dall'area di gioco è SIGQUIT imposto le vite della bomba a 0
    if(signum == SIGQUIT) { b_alive = 0; }
}
/**
 * Funzione di gestione della bomba lanciata da un alieno
 * @param pipeout pipe che scrive nell'area di gioco
 * @param x posizione iniziale x
 * @param y posizione iniziale y
 */
void bomb_task(int pipeout, int x, int y){
    Object bomb;

    //  Registro l'handler per il segnale di QUIT
    signal(SIGQUIT, bomb_handler);

    //  inizializzazione oggetto bomba
    bomb.ID         = BOMB;
    bomb.x          = x;
    bomb.y          = y;
    bomb.pid        = getpid();
    bomb.size       = 1;
    bomb.pipes[0]   = bomb.pipes[1] = 0;    //  non viene utilizzata

    // 	scrive in pipe la posizione iniziale
    write(pipeout, &bomb, sizeof(Object));

    // Ciclo finche la bomba non esce dallo schermo ed è ancora viva
    while((!(bomb.x < 0 || bomb.y < 0 || bomb.x > MAXX || bomb.y > MAXY)) && b_alive){
        bomb.y += 1;    //  incremento la coordinata y

        //  scrivo aggiornamenti oggetto in pipe
        write(pipeout, &bomb, sizeof(Object));
        usleep(SLEEP_BOMB);
    }

    //  la bomba deve essere cancellato dall'area di gioco
    bomb.ID = DELETE;   //  imposto ID oggetto a DELETE
    write(pipeout, &bomb, sizeof(Object));  //  scrivo aggiornamenti oggetto in pipe

    
    close(pipeout); //  chiudo la pipe dalla modalita scrittura in modo che il processo non possa più scrivere in pipe
    exit(0);
}
