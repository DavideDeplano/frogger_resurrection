#include "header.h"

void* grenade(void *arg) {
    // Inizializzazione delle struct per le coordinate della rana e delle granate
    struct Coordinates frog, grenade, new;

    // Inizializzazione delle variabili per la grandezza dello schermo e della variabile della coordinata
    int maxx = (N_DENS * L_FROG) + (4 * L_FROG) + 2;
    int x;

    int id = (int)(intptr_t)arg; // Recupera l'id passato come argomento

    // Inizializzazione variabile per la velocità di movimento
    int udelay = 100000;

    // Inizializzazione variabili del numero di elementi prodotti o letti 
    int count_read = 0, count_generated = 0;

    // Thread granata generato e attivo invia il segnale a control
    grenade.c = GRENADE_CREATED;
    while(count_generated < 1) {
        wait_producer(&sem_empty);
        buffer[i_write] = grenade;
        i_write = (i_write + 1) % DIM_BUFFER;
        signal_producer(&sem_full);
        count_generated++;
    }
    count_generated = 0;

    // Legge le coordinate della rana per generare la posizione dei proiettili
    while (count_read < 1) {
        wait_consumer(&sem_full_grenade);
        frog = buffer_grenade[i_read_grenade];
        i_read_grenade = (i_read_grenade + 1) % DIM_BUFFER_GRENADE;
        signal_consumer(&sem_empty_grenade);
        count_read++;
    }
    count_read = 0;

    if (frog.c == COORDINATE_X_FROG_GRENADE && frog.dir == id) { // Se corrisponde al processo
        if (id == 0) {
            x = frog.x; // Aggiorna la posizione iniziale della granata sinistra
        }
        if (id == 1) {
            x = frog.x + L_FROG - 1; // Aggiorna la posizione iniziale della granata destra
        }
    } 

    if (id == 0 && x != frog.x) {
        x = frog.x;
    }

    if (id == 1 && x != frog.x + L_FROG - 1) {
         x = frog.x + L_FROG - 1;
    }

    while (1) {
        // Se la posizione della granata è fuori dallo schermo esce dal ciclo e termina il thread
        if (x < 0 || x >= maxx) {break;}

        // Movimento della granata
        if (id == 0) { // Granata sinistra
            if (x >= 0) { // Se la coordinata è maggiore o uguale a zero
                x--; // La granata va verso sinistra

                // Invia la coordinata a control
                grenade.x = x;
                grenade.c = COORDINATES_X_GRENADE_LEFT;

                while(count_generated < 1) {
                    wait_producer(&sem_empty);
                    buffer[i_write] = grenade;
                    i_write = (i_write + 1) % DIM_BUFFER;
                    signal_producer(&sem_full);
                    count_generated++;
                }

                count_generated = 0;

                // Prova a leggere il buffer se ci sono coordinate per la terminazione del thread senza bloccarlo
                if (sem_trywait(&sem_full_grenade) == 0) {
                    new = buffer_grenade[i_read_grenade];
                    i_read_grenade = (i_read_grenade + 1) % DIM_BUFFER_GRENADE;
                    signal_consumer(&sem_empty_grenade);
                    if (new.c == TERMINATE_GRENADE) {
                        if (new.dir == id) { // Se l'id corrisponde al thread

                            // Aggoiorna la posizione della granata
                            x = new.x;
                            
                            // Invia le coordinate aggiornate a control
                            grenade.x = x;
                            grenade.c = COORDINATES_X_GRENADE_LEFT;

                            while(count_generated < 1) {
                                wait_producer(&sem_empty);
                                buffer[i_write] = grenade;
                                i_write = (i_write + 1) % DIM_BUFFER;
                                signal_producer(&sem_full);
                                count_generated++;
                            }
                            count_generated = 0;

                            break; // Esce dal ciclo
                        }
                    }
                }

                usleep(udelay); // Pausa per l'invio delle coordinate
            } else break; // Altrimenti esce dal ciclo e termina il thread

        } else { // Granata destra
            if (x < maxx) { // Se la coordinata è minore di maxx 
                x++; // La granata va verso destra
                
                // Invia la coordinata a control
                grenade.x = x;
                grenade.c = COORDINATES_X_GRENADE_RIGHT;

                while(count_generated < 1) {
                    wait_producer(&sem_empty);
                    buffer[i_write] = grenade;
                    i_write = (i_write + 1) % DIM_BUFFER;
                    signal_producer(&sem_full);
                    count_generated++;
                }

                count_generated = 0;

                // Prova a leggere il buffer se ci sono coordinate per la terminazione del thread senza bloccarlo
                if (sem_trywait(&sem_full_grenade) == 0) {
                    new = buffer_grenade[i_read_grenade];
                    i_read_grenade = (i_read_grenade + 1) % DIM_BUFFER_GRENADE;
                    signal_consumer(&sem_empty_grenade);
                    if (new.c == TERMINATE_GRENADE) {
                        if (new.dir == id) { // Se l'id corrisponde al thread

                            // Aggoiorna la posizione della granata
                            x = new.x;
                            
                            // Invia la coordinata a control
                            grenade.x = x;
                            grenade.c = COORDINATES_X_GRENADE_RIGHT;

                            while(count_generated < 1) {
                                wait_producer(&sem_empty);
                                buffer[i_write] = grenade;
                                i_write = (i_write + 1) % DIM_BUFFER;
                                signal_producer(&sem_full);
                                count_generated++;
                            }
                            count_generated = 0;

                            break; // Esce dal ciclo
                        }
                    }   
                }
                usleep(udelay); // Pausa per l'invio delle coordinate
            } else break; // Altrimenti esce dal ciclo e termina il processo
        }
    }
    pthread_exit(NULL); // Termina il processo della granata
}
    
