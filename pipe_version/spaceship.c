#include "spaceship.h"


// --------------------------------------------------- SPACESHIP
// Variabile globale per la vita della spaceship
int s_alive = 1;
/** 
 * Handler della navicella per il segnale SIGQUIT 
 * @param signum segnale 
 */
void spaceship_handler(int signum){
    //  se il segnale che arriva al processo dall'area di gioco è SIGQUIT imposto le vite della spaceship a 0
    if(signum == SIGQUIT) { s_alive = 0; }
}
/**
 * Funzione di gestione della spaceship guidata dall'utente
 * @param pipeout pipe che scrive nell'area di gioco
 */
void spaceship_task(int pipeout){
    Object spaceship;   //  oggetto spaceship
    pid_t   missileL,   //  pid missile sinistro
            missileR;   //  pid missile destro
    char ch;            //  variabile per comando acquisito da tastiera        

    //  Registro l'handler per il segnale di QUIT
    signal(SIGQUIT, spaceship_handler);

    // inizializzazione oggetto spaceship
    spaceship.ID 		= SPACESHIP;				//	valore associato al tipo di oggetto
    spaceship.x	 		= MAXX/2;		            //	coordinata x
    spaceship.y 		= MAXY-SPACESHIP_SIZE;		//	coordinata y
    spaceship.size 		= SPACESHIP_SIZE;			// 	dimensione oggetto
    spaceship.life 		= 1;			            //	vite spaceship
    spaceship.pid 		= getpid();					//	acquisisco il numero del processo
    spaceship.pipes[0] 	= spaceship.pipes[1] = 0;	//	pipe, non vengono utilizzate


    // 	scrive in pipe la posizione iniziale
    write(pipeout, &spaceship, sizeof(Object));


    //  finchè la spaceship è in vita continuo nel while
    while(s_alive){

        //  acquisico carattere per movimento o sparo
        ch = getchar();
        switch(ch){
            case SPACEBAR:
                //  processo missile sx
                missileL = fork();  
                switch(missileL){
                    case -1:
                        perror("Impossibile creare il processo missile.");
                        exit(-1);
                        
                    case 0:
                        missile_task(pipeout, LEFT, spaceship.x + spaceship.size / 2, spaceship.y - 1);
                        
                }
                //  processo missile dx
                missileR = fork();  
                switch(missileR){
                    case -1:
                        perror("Impossibile creare il processo missile.");
                        exit(-1);

                    case 0:
                        missile_task(pipeout, RIGHT, spaceship.x + spaceship.size / 2, spaceship.y - 1);
                        
                }
                break;
            case SX:
                if((spaceship.x - 1) >= 0) { spaceship.x -= 1; }
                break;
            case DX:
                if((spaceship.x + 1) < (MAXX - spaceship.size)) { spaceship.x += 1; }
                break;
        }
        //  scrivo aggiornamenti oggetto in pipe
        write(pipeout, &spaceship, sizeof(Object));
    }

    close(pipeout); //  chiudo la pipe dalla modalita scrittura in modo che il processo non possa più scrivere in pipe
    exit(0);
}


/// --------------------------------------------------- MISSILE
// Variabile globale per la vita della spaceship
int m_alive = 1;
/** 
 * Handler del missile per il segnale SIGQUIT 
 * @param signum segnale 
 */
void missile_handler(int signum){
    //  se il segnale che arriva al processo dall'area di gioco è SIGQUIT imposto le vite del missile a 0
    if(signum == SIGQUIT){ m_alive = 0; }
}
/**
 * Funzione di gestione del missile lanciato dalla navicella
 * @param pipeout pipe che scrive nell'area di gioco
 * @param dir direzione 
 * @param x posizione iniziale x
 * @param y posizione iniziale y
 */
void missile_task(int pipeout, Direction dir, int x, int y){
    Object missile; //  oggetto missile

    //  Registro l'handler per il segnale di QUIT
    signal (SIGQUIT, missile_handler);

    //  inizializzazione oggetto missile
    missile.ID          = MISSILE;   
    missile.x           = x;            
    missile.y           = y;            
    missile.pid         = getpid();     
    missile.size        = 1;            
    missile.pipes[0] = missile.pipes[1] = 0;    //  non viene utilizzata

    // 	scrive in pipe la posizione iniziale
    write(pipeout, &missile, sizeof(Object));


    // Eseguo sino a che l'oggetto non esce dallo schermo, o sino a che non riceve un segnale d'uscita
    while((! (missile.x < 0 || missile.y < 0 || missile.x > MAXX || missile.y > MAXY)) && m_alive){
        missile.y -= 1; //  decremento coordinata y missile

        // A seconda della direzione, mi sposto anche in orizzontale
        switch(dir){
            case LEFT:
                missile.x -= 1;
                break;
            case RIGHT:
                missile.x += 1;
                break;
        }

        //  scrivo aggiornamenti oggetto in pipe
        write(pipeout, &missile, sizeof(Object)); 
        usleep(SLEEP_UTIME);
    }

    //  il missile deve essere cancellato dall'area di gioco
    missile.ID = DELETE;    //  imposto ID oggetto a DELETE
    write(pipeout, &missile, sizeof(Object));   //  scrivo aggiornamenti oggetto in pipe

    
    close(pipeout); //  chiudo la pipe dalla modalita scrittura in modo che il processo non possa più scrivere in pipe
    exit(0);
}
