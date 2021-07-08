

spaceinvaders: main.o control.o spaceship.o enemy.o
	gcc -o spaceinvaders main.o control.o spaceship.o enemy.o -lncurses -lpthread


main.o: main.c spaceinvaders.h
	gcc -c -o main.o main.c 

control.o: control.c control.h spaceinvaders.h
	gcc -c -o control.o control.c

spaceship.o: spaceship.c spaceship.h spaceinvaders.h
	gcc -c -o spaceship.o spaceship.c

enemy.o: enemy.c enemy.h spaceinvaders.h
	gcc -c -o enemy.o enemy.c

clean:
	rm *.o spaceinvaders
