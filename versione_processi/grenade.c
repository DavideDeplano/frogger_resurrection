#include "header.h"

void grenade(int id, int pipe_p[], int pipe_g[][2]) {
    close(pipe_p[0]); // Chiude la pipe to parent in lettura
    close(pipe_g[0][1]); // Chiude la prima pipe to grenade in scrittura
    close(pipe_g[1][1]); // Chiude la seconda pipe to grenade in scrittura

    // Inizializzazione delle struct per le coordinate della rana e delle granate
    struct Coordinates frog, grenade, new;

    // Inizializzazione delle variabili per la grandezza dello schermo e della variabile della coordinata
    int maxx = (N_DENS * L_FROG) + (4 * L_FROG) + 2;
    int x;

    // Inizializzazione variabile per la velocità di movimento
    int udelay = 100000;

    // Processo granata generato e attivo invia il segnale a control
    grenade.c = GRENADE_CREATED;
    write(pipe_p[1], &grenade, sizeof(grenade));
    
    // Legge le coordinate della rana per generare la posizione dei proiettili
    while (read(pipe_g[id][0], &frog, sizeof(frog)) <= 0) {
        usleep(1000);
    }

    if (frog.c == COORDINATE_X_FROG_GRENADE && frog.dir == id) { // Se corrisponde al processo
        if (id == 0) {
            x = frog.x; // Aggiorna la posizione iniziale della granata sinistra
        } else {
            x = frog.x + L_FROG - 1; // Aggiorna la posizione iniziale della granata destra
        }
    }
    
    if(id == 0 && x != frog.x) {
        x = frog.x;
    }

    if(id == 1 && x != frog.x + L_FROG - 1) {
        x = frog.x + L_FROG - 1;
    }

    while (1) {
        // Se la posizione della granata è fuori dallo schermo esce dal ciclo e termina il processo
        if (x < 0 || x >= maxx) {break;}

        // Legge la pipe se c'è il segnale per terminare il processo
        if (read(pipe_g[id][0], &new, sizeof(new)) > 0) {
            if (new.c == TERMINATE_GRENADE) {
                if (new.dir == id) { // Se l'id corrisponde al processo

                    // Aggoiorna la posizione della granata
                    x = new.x;

                    // Invia le coordinate aggiornate a control
                    grenade.x = x;
                    grenade.c = COORDINATES_X_GRENADE_LEFT;

                    write(pipe_p[1], &grenade, sizeof(grenade));

                    // Esce dal ciclo e termina il processo
                    break;
                } else {
                    // Altrimenti aggiorna la posizione della granata e invia la posizione a control
                    x = new.x;

                    grenade.x = x;
                    grenade.c = COORDINATES_X_GRENADE_RIGHT;

                    write(pipe_p[1], &grenade, sizeof(grenade));

                    break; // Esce dal ciclo
                }
            }
        }

        // Movimento della granata
        if (id == 0) { // Granata sinistra
            if (x >= 0) { // Se la coordinata è maggiore o uguale a zero
                x--; // La granata va verso sinistra

                // Invia la coordinata a control
                grenade.x = x;
                grenade.c = COORDINATES_X_GRENADE_LEFT;

                write(pipe_p[1], &grenade, sizeof(grenade));
                usleep(udelay); // Pausa per l'invio delle coordinate
            } else break; // Altrimenti esce dal ciclo e termina il processo
        } else { // Granata destra
            if (x < maxx) { // Se la coordinata è minore di maxx 
                x++; // La granata va verso destra

                // Invia la coordinata a control
                grenade.x = x;
                grenade.c = COORDINATES_X_GRENADE_RIGHT;

                write(pipe_p[1], &grenade, sizeof(grenade));
                usleep(udelay); // Pausa per l'invio delle coordinate
            } else break; // Altrimenti esce dal ciclo e termina il processo
        }
    }
    exit(0); // Termina il processo della granata
}
