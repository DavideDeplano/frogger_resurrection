#include "header.h"

void *bullet(void* unused) {
    // Inizailizzazione delle struct per le coordinate del coccodrillo e del proiettile
    struct Coordinates crocodile, bullet, new;
    
     // Inizializzazione della variabile per la grandezza dello schermo
    int maxx = (N_DENS * L_FROG) + (4 * L_FROG) + 2;

    // Inizializzazione delle variabili per le coordinate e la direzione del proiettile
    int y, x, dir;

    // Inizializzazione della velocità del movimento
    int udelay = 100000;

    // Inizializzazione variabili del numero di elementi prodotti o letti
    int count_read = 0, count_generated = 0;

    // Thread proiettile generato e attivo invia segnale a control 
    bullet.c = BULLET_CREATED;
    while(count_generated < 1) {
        wait_producer(&sem_empty);
        buffer[i_write] = bullet;
        i_write = (i_write + 1) % DIM_BUFFER;
        signal_producer(&sem_full);
        count_generated++;
    }
    count_generated = 0;
    
    // Legge il buffer bullet e riceve le coordinate del coccodrillo che deve sparare
    while (count_read < 1) {
        wait_consumer(&sem_full_bullet);
        crocodile = buffer_bullet[i_read_bullet];
        i_read_bullet = (i_read_bullet + 1) % DIM_BUFFER_BULLET;
        signal_consumer(&sem_empty_bullet);
        count_read++;
    }
    count_read = 0;

    if (crocodile.c == COORDINATE_X_CROCODILE_BULLET) {
        y = crocodile.y;
        x = crocodile.x;
        dir = crocodile.dir;
    }

    while(1) {
        if (dir == 0) { // Se la direzione è da sinistra a destra
            if (x < maxx) { // Se la posizione è minore di maxx
                x++; // Aumenta di uno verso destra

                // Invia le coordinate a control
                bullet.y = y;
                bullet.x = x;
                bullet.c = COORDINATES_BULLET;
                bullet.dir = dir;
                
                while(count_generated < 1) {
                    wait_producer(&sem_empty);
                    buffer[i_write] = bullet;
                    i_write = (i_write + 1) % DIM_BUFFER;
                    signal_producer(&sem_full);
                    count_generated++;
                }
                count_generated = 0;
                
                // Prova a leggere il buffer controlla se ha ricevuto il comando di terminazione senza bloccare il thread 
                if (sem_trywait(&sem_full_bullet) == 0) {
                    new = buffer_bullet[i_read_bullet];
                    i_read_bullet = (i_read_bullet + 1) % DIM_BUFFER_BULLET;
                    signal_consumer(&sem_empty_bullet);
                       
                    if (new.c == TERMINATE_BULLET && new.dir == 0) {
                        // Aggiorna la posizione fuori dallo schermo
                        x = new.x;

                        // Invia la posizione aggiornata a control
                        bullet.x = x;
                        bullet.c = COORDINATES_BULLET;

                        while(count_generated < 1) {
                            wait_producer(&sem_empty);
                            buffer[i_write] = bullet;
                            i_write = (i_write + 1) % DIM_BUFFER;
                            signal_producer(&sem_full);
                            count_generated++;
                        }
                        count_generated = 0;
                    }
                    break; // Esce dal ciclo e termina il thread
                }

                usleep(udelay); // Ritardo del movimento
            } else break; // Esce dal ciclo e termina il processo
        } else { // Altrimenti se la direzione è da destra a sinistra
            if (x >= 0) { // Se la posizione è maggiore o uguale a zero
                x--; // Diminuisce la coordinata di uno

                // Invia le coordinate aggiornate a control
                bullet.y = y;
                bullet.x = x;
                bullet.c = COORDINATES_BULLET;
                bullet.dir = dir;
                
                while(count_generated < 1) {
                    wait_producer(&sem_empty);
                    buffer[i_write] = bullet;
                    i_write = (i_write + 1) % DIM_BUFFER;
                    signal_producer(&sem_full);
                    count_generated++;
                }
                count_generated = 0;
                
                // Prova a leggere il buffer controlla se ha ricevuto il comando di terminazione senza bloccare il thread 
                if (sem_trywait(&sem_full_bullet) == 0) {
                    new = buffer_bullet[i_read_bullet];
                    i_read_bullet = (i_read_bullet + 1) % DIM_BUFFER_BULLET;
                    signal_consumer(&sem_empty_bullet);
                     
                    if (new.c == TERMINATE_BULLET && new.dir == 1) {
                        // Aggiorna la posizione fuori dallo schermo
                        x = new.x;
                    
                        // Invia le coordinate aggiornate a control
                        bullet.x = x;
                        bullet.c = COORDINATES_BULLET;

                        while(count_generated < 1) {
                            wait_producer(&sem_empty);
                            buffer[i_write] = bullet;
                            i_write = (i_write + 1) % DIM_BUFFER;
                            signal_producer(&sem_full);
                            count_generated++;
                        }
                        count_generated = 0;
                    }  
                    break; // Esce dal ciclo e termina il thread
                }

                usleep(udelay); // Ritardo per il movimento
            } else break; // Altrimenti esce dal ciclo e termina il processo
        }
    }
    pthread_exit(NULL); // Termina il thread
}

