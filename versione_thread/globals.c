#include "header.h"

// Assegnamento delle dimensioni dei buffer
struct Coordinates buffer[DIM_BUFFER], buffer_frog[DIM_BUFFER_FROG], buffer_crocodile[DIM_BUFFER_CROCODILE], buffer_grenade[DIM_BUFFER_GRENADE], buffer_bullet[DIM_BUFFER_BULLET];

// Inizializzazione semafori
sem_t sem_empty, sem_full;
sem_t sem_empty_frog, sem_full_frog;
sem_t sem_empty_crocodile, sem_full_crocodile;
sem_t sem_empty_grenade, sem_full_grenade;
sem_t sem_empty_bullet, sem_full_bullet;

// Assegnamento valori degli indici di lettura e scrittura
int i_write = 0, i_read = 0;
int i_write_frog = 0, i_read_frog = 0;
int i_write_crocodile = 0, i_read_crocodile = 0;
int i_write_grenade = 0, i_read_grenade = 0;
int i_write_bullet = 0, i_read_bullet = 0;


