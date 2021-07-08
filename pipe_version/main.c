#include "spaceinvaders.h"


/// --------------------------------------------------- MAIN
int main(){
	int i,				//  indice
		startx = 0,		//	coordinata x di partenza enemy
		starty = 2;		//	coordinata y di partenza enemy	
	int fd[2],			//	pipe principale
        fd_coll[2]; 	//	pipe collisioni
	


	pid_t pid_spaceship;	// Pid 	navicella
	pid_t pid_enemy;		// Pid 	alieno	


	// 	Creazione della pipe
	if(pipe(fd) == -1) {
		perror("Errore nella creazione della pipe.");
		exit(-1);
	}


	// 	Creo processo per la navicella
	pid_spaceship = fork();
	switch(pid_spaceship){
		case -1:
			perror("Impossibile creare il processo SpaceShip.");
			exit(-1);	

		case 0:
			close(fd[0]);			//	chiudo descrittore pipe in lettura
			spaceship_task(fd[1]);	// 	funzione per il movimento della spaceship con descrittore pipe aperto in scrittura
	}


	//	itero per il numero di enemy
	for(i = 0; i < M; i++){
		pipe(fd_coll);	//	creo pipe per la gestione delle collisioni

		// 	Rendo non bloccante la pipe per riceve continuamente lo stato delle collisioni
    	fcntl(fd_coll[0], F_SETFL, fcntl(fd_coll[0], F_GETFL) | O_NONBLOCK);
		
		startx = (ENEMY_SIZE + 2) * (i + 1);	//	coordinate x enemy i
		
		pid_enemy = fork(); //  creo processo alieno
		switch(pid_enemy){
			case -1:
				perror("Impossibile creare il processo Enemy.");
				exit(-1);

			case 0:
				close(fd[0]);		//	chiudo pipe principale in lettura
				close(fd_coll[1]);	// 	apro pipe per le collisioni in scrittura
				
				//	funzione per il movimento di un enemy i, se i pari allora la dir è destra altrimenti sinistra
				if(i%2 == 0){ enemy_task(fd[1], startx, starty, fd_coll, RIGHT, LEVEL1); }
				else{ enemy_task(fd[1], MAXX - startx, starty, fd_coll, LEFT, LEVEL1); }
		}
		
		if(i%2 == 1){ starty += ENEMY_SIZE; }	//	se i dispari incremento la y per la generazione dei successivi enemy
		close(fd_coll[0]); //  chiudo pipe collisioni in lettura	
	}

	init_scr();	//	inzializzazione ncurses
	
	close(fd[1]);			//	chiudo pipe principale in scrittura
	control_task(fd[0]);	//	chiamo la funzione di controllo, con pipe aperta in lettura

	end_scr();			//	terminazione ncurses
	stampa(result);	//	stampa scritta WIN o GAMEOVER
	
	return 0;
}

