#include "header.h"

void bullet(int pipe_p[], int pipe_b[]) {
    close(pipe_p[0]); // Chiude la pipe to parent in lettura
    close(pipe_b[1]); // Chiude la pipe to bullet in scrittura

    // Inizailizzazione delle struct per le coordinate del coccodrillo e del proiettile
    struct Coordinates crocodile, bullet, new;

    // Inizializzazione della variabile per la grandezza dello schermo
    int maxx = (N_DENS * L_FROG) + (4 * L_FROG) + 2;

    // Inizializzazione delle variabili per le coordinate e la direzione del proiettile
    int y, x, dir;

    // Inizializzazione della velocità del movimento
    int udelay = 100000;

    // Processo proiettile generato e attivo invia segnale a control 
    bullet.c = BULLET_CREATED;
    write(pipe_p[1], &bullet, sizeof(bullet));

    // Legge la pipe to bullet e riceve le coordinate del coccodrillo che deve sparare
    if (read(pipe_b[0], &crocodile, sizeof(crocodile)) > 0) {
        if (crocodile.c == COORDINATE_X_CROCODILE_BULLET) {
            y = crocodile.y;
            x = crocodile.x;
            dir = crocodile.dir;
        }
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
                write(pipe_p[1], &bullet, sizeof(bullet));

                // Se riceve il comando di terminazione
                if (read(pipe_b[0], &new, sizeof(new)) > 0) {
                    if (new.c == TERMINATE_BULLET) {

                        // Aggiorna la posizione fuori dallo schermo
                        x = new.x;
                        
                        // Invia la posizione aggiornata a control
                        bullet.x = x;
                        bullet.c = COORDINATES_BULLET;

                        write(pipe_p[1], &bullet, sizeof(bullet));

                        // Esce dal ciclo e termina il processo
                        break;
                    }
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
                write(pipe_p[1], &bullet, sizeof(bullet));

                // Se riceve il comando di terminazione
                if (read(pipe_b[0], &new, sizeof(new)) > 0) {
                    if (new.c == TERMINATE_BULLET) {

                        // Aggiorna la posizione fuori dallo schermo
                        x = new.x;

                        // Invia le coordinate aggiornate a control
                        bullet.x = x;
                        bullet.c = COORDINATES_BULLET;

                        write(pipe_p[1], &bullet, sizeof(bullet));

                        // Esce dal ciclo e termina il processo
                        break;
                    }
                }

                usleep(udelay); // Ritardo per il movimento
            } else break; // Altrimenti esce dal ciclo e termina il processo
        }
    }
    exit(0); // Termina il processo 
}