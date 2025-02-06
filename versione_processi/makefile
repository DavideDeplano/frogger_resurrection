frogger_resurration.out: clean main.o frog.o crocodile.o control.o grenade.o bullet.o audio.o server.o
	gcc main.o frog.o crocodile.o control.o grenade.o bullet.o audio.o server.o -o frogger_resurration.out -lncurses -lSDL2 -lSDL2_mixer 

main.o: main.c header.h
	gcc -c main.c

frog.o: frog.c header.h
	gcc -c frog.c

crocodile.o: crocodile.c header.h
	gcc -c crocodile.c

control.o: control.c header.h
	gcc -c control.c

grenade.o: grenade.c header.h
	gcc -c grenade.c

bullet.o: bullet.c header.h
	gcc -c bullet.c

audio.o: audio.c header.h
	gcc -c audio.c

server.o: server.c header.h
	gcc -c server.c

clean: 
	rm -f *.o