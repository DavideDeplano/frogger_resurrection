#include "header.h"

// Inizializzazione semafori
void init_sync() {
    sem_init(&sem_full, 0, 0); 
    sem_init(&sem_full_frog, 0, 0);
    sem_init(&sem_full_crocodile, 0, 0);
    sem_init(&sem_full_grenade, 0, 0);
    sem_init(&sem_full_bullet, 0, 0);

    sem_init(&sem_empty, 0, DIM_BUFFER); 
    sem_init(&sem_empty_frog, 0, DIM_BUFFER_FROG);
    sem_init(&sem_empty_crocodile, 0, DIM_BUFFER_CROCODILE);
    sem_init(&sem_empty_grenade, 0, DIM_BUFFER_GRENADE);
    sem_init(&sem_empty_bullet, 0, DIM_BUFFER_BULLET);
}

// Distruzione semafori
void destroy_sync() {
    sem_destroy(&sem_full);
    sem_destroy(&sem_full_frog);
    sem_destroy(&sem_full_crocodile);
    sem_destroy(&sem_full_grenade);
    sem_destroy(&sem_full_bullet);

    sem_destroy(&sem_empty);
    sem_destroy(&sem_empty_frog);
    sem_destroy(&sem_empty_crocodile);
    sem_destroy(&sem_empty_grenade);
    sem_destroy(&sem_empty_bullet);
}

// Funzioni wait e signal passando come argomento il semaforo
void wait_producer(sem_t *sem_empty) { 
    sem_wait(sem_empty); 
}

void signal_producer(sem_t *sem_full) { 
    sem_post(sem_full);
}

void wait_consumer(sem_t *sem_full) { 
    sem_wait(sem_full);
}

void signal_consumer(sem_t *sem_empty) { 
    sem_post(sem_empty); 
}
