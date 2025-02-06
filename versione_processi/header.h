// Librerie
#include <fcntl.h>
#include <ncurses.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Macro
#define H_LIFES_TIME 3 // Altezza finestra vite e tempo di gioco
#define H_PAVEMENT 6 // Altezza finestra marciapiede
#define H_RIVER 16 // Altezza finestra fiume
#define H_GRASS 6 // Altezza finestra prato
#define H_DENS 12 // Altezza finestra tane
#define H_SCORE 3 // Altezza finestra punteggio

#define H_FROG 2 // Altezza rana
#define L_FROG 12 // Lunghezza rana
#define L_CROCODILE 24 // Lunghezza coccodrillo
#define N_DENS 5 // Numero di tane
#define N_CROCODILE 16 // Numero di coccodrilli
#define N_MINE 5 // Numero di mine

// Nome usato per le coordinate
#define COORDINATES_FROG 'F' // Rana -> Controllo
#define COORDINATES_CROCODILE 'C' // Coccodrillo -> Controllo
#define GRENADE_CREATED 'E' // Granata -> Controllo
#define COORDINATES_X_GRENADE_LEFT 'L' // Granata -> Controllo
#define COORDINATES_X_GRENADE_RIGHT 'R' // Granata -> Controllo
#define COORDINATES_FROG_GRENADE 'G' // Rana -> Controllo 
#define BULLET_CREATED 'U' // Proiettile -> Controllo
#define COORDINATES_BULLET 'T' // Proiettile -> Controllo

#define FROG_PAVEMENT 'p' // Controllo -> Rana
#define FROG_RIVER 'r' // Controllo -> Rana
#define FROG_GRASS 'g' // Controllo -> Rana
#define FROG_DENS 'd' // Controllo -> Rana
#define FROG_BACK 'b' // Controllo -> Rana
#define FROG_RESET 'n' // Controllo -> Rana

#define BULLET_PROCESS 'y' // Controllo -> Coccodrillo
#define CROCODILE_DIRECTION 'c' // Controllo -> Coccodrillo

#define COORDINATE_X_FROG_GRENADE 'x' // Controllo -> Granata
#define TERMINATE_GRENADE 'u' // Controllo -> Granata

#define COORDINATE_X_CROCODILE_BULLET 'w' // Controllo -> Proiettile
#define TERMINATE_BULLET 't' // Controllo -> Proiettile

#define MAX_PENDING 1 // Numero massimo di richieste di connessione in coda per il socket
#define MAX_CONNECTIONS 1 // Numero massimo di connesioni contemporanee gestote dal socket

// Struttura per la trasmissione delle coordinate
struct Coordinates {
    int y;
    int x;
    char c;
    int dir;
};

// Struttura vittoria/sconfitta e punteggio
struct Score {
    int result;
    int score;
};

void frog(int[], int[], int[][2]);
void crocodile(int, int[], int[], int[], int);
void grenade(int, int[], int[][2]);
void bullet(int[], int[]);
struct Score control(int[], int[], int[], int[][2], int[], int, int);
int server(int*);

void initAudio();
void playSound(Mix_Chunk *sound, int);
Mix_Chunk *loadSound(const char *file);
void freeSound(Mix_Chunk *sound);
void closeAudio();
