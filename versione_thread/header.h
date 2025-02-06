// Librerie 
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>

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
#define COORDINATES_X_GRENADE_LEFT 'L' // Granata -> Controllo
#define COORDINATES_X_GRENADE_RIGHT 'R' // Granata -> Controllo
#define GRENADE_CREATED 'U' // Granata -> Control
#define COORDINATES_FROG_GRENADE 'G' // Rana -> Controllo 
#define BULLET_CREATED 'T' // Proiettile -> Controllo
#define COORDINATES_BULLET 'B' // Proiettile -> Controllo

#define FROG_PAVEMENT 'p' // Controllo -> Rana
#define FROG_RIVER 'r' // Controllo -> Rana
#define FROG_GRASS 'g' // Controllo -> Rana
#define FROG_DENS 'd' // Controllo -> Rana
#define FROG_BACK 'b' // Controllo -> Rana
#define FROG_RESET 'n' // Controllo -> Rana
#define BULLET_THREAD 'y' // Controllo -> Coccodrillo

#define CROCODILE_DIRECTION 'c' // Controllo -> Coccodrillo

#define COORDINATE_X_FROG_GRENADE 'x' // Controllo -> Granata
#define TERMINATE_GRENADE 'u' // Controllo -> Granata

#define COORDINATE_X_CROCODILE_BULLET 'w' // Controllo -> Proiettile
#define TERMINATE_BULLET 't' // Controllo -> Proiettile

#define MAX_PENDING 1 // Numero massimo di richieste di connessione in coda per il socket
#define MAX_CONNECTIONS 1 // Numero massimo di connesioni contemporanee gestote dal socket

// Dimensione dei buffer circolari
#define DIM_BUFFER 35
#define DIM_BUFFER_FROG 2
#define DIM_BUFFER_CROCODILE 16
#define DIM_BUFFER_GRENADE 10
#define DIM_BUFFER_BULLET 5

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

struct Crocodile_args {
    int id;
    int udelay;
};

// Inizializzazione della dimensione dei buffer
extern struct Coordinates buffer[DIM_BUFFER], buffer_frog[DIM_BUFFER_FROG], buffer_crocodile[DIM_BUFFER_CROCODILE], buffer_grenade[DIM_BUFFER_GRENADE], buffer_bullet[DIM_BUFFER_BULLET];

// Inizializzazione dei semafori
extern sem_t sem_empty, sem_full;
extern sem_t sem_empty_frog, sem_full_frog;
extern sem_t sem_empty_crocodile, sem_full_crocodile;
extern sem_t sem_empty_grenade, sem_full_grenade;
extern sem_t sem_empty_bullet, sem_full_bullet;

// Inizializzazione delle variabili indice lettura e scrittura
extern int i_write, i_read;
extern int i_write_frog, i_read_frog;
extern int i_write_crocodile, i_read_crocodile;
extern int i_write_grenade, i_read_grenade;
extern int i_write_bullet, i_read_bullet;

void *frog(void *);
void *crocodile(void *arg);
void *grenade(void *arg);
void *bullet(void *);
struct Score control(int, int);
int server(int*);

void init_sync();
void destroy_sync();
void wait_producer(sem_t*);
void signal_producer(sem_t*);
void wait_consumer(sem_t*);
void signal_consumer(sem_t*);

void initAudio();
void playSound(Mix_Chunk *sound, int);
Mix_Chunk *loadSound(const char *file);
void freeSound(Mix_Chunk *sound);
void closeAudio();
