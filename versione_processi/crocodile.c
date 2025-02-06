#include "header.h"

void crocodile(int id, int pipe_p[], int pipe_c[], int pipe_b[], int base) {
    close(pipe_p[0]); // Chiude la pipe to parent in lettura
    close(pipe_c[1]); // Chiude la pipe to crocodile in scrittura

    // Inizializzazione pid per il proiettile
    pid_t pid_bullet;

    // Inizializzazione delle struct per il movimento del coccodrillo e per generare il proiettile
    struct Coordinates command;

    // Inizializzazione della variabile per la grandezza dello schermo
    int maxx = (N_DENS * L_FROG) + (4 * L_FROG) + 2; 

    // Inizializzazione per la posizione e la direzione del coccodrillo
    int x, dir_random;                                          

    // Inizializzazione della variabile per identificare il primo e il secondo coccodrillo della riga nel fiume
    int pair_id = id / 8;                   

    // Inizializzazione della variabile per identificare gli 8 gruppi di due coccodrilli distribuiti sulle 8 righe del fiume       
    int group_id = id % 8;                         

    // Inizializzazione della variabile per regolare la velocità dei coccodrilli con base uguale per tutti e differenziata per gli 8 flussi del fiume
    int udelay = base + group_id * rand() % (100000 - 50000) + 50000;

    while (1) {
        // Legge dalla pipe control -> crocodile
        if (read(pipe_c[0], &command, sizeof(command)) > 0) {
            if (command.c == CROCODILE_DIRECTION) { // Comando di nuova manche ricevuto
                // Configura la direzione in base all'ID del coccodrillo
                if (command.dir == 0) { // Se il comando random di control invia 0 
                    if(id % 2 == 0) {  // Se l'id del coccodrillo è pari
                        dir_random = 0; // Allora la direzione è da destra a sinistra
                    } else {
                        dir_random = 1; // Altrimenti è da sinistra a destra
                    }
                } else { // Altrimenti se il comando random di control invia 1
                    if(id % 2 == 0) { // Se l'id del coccodrillo è pari
                        dir_random = 1; // Allora la direzione è da sinistra a destra
                    } else {
                        dir_random = 0; // Altrimenti è da destra a sinistra
                    }
                }
                
                // Configura la posizione iniziale in base alla direzione
            if (pair_id == 0){ // Se è il primo coccodrillo del gruppo
                    if (dir_random == 0) { // Se la direzione è sx → dx
                        x = -(L_CROCODILE); // Partenza fuori dallo schermo a sinistra
                    } else { // Se la direzione è dx → sx
                        x = maxx; // Partenza fuori dallo schermo a destra
                    }
                } else { 
                    // Altrimenti se è il secondo coccodrillo del gruppo aggiunge spazio aggiuntivo per non farli sovrapporre
                    if(dir_random == 0) {
                        x = -(L_CROCODILE * 3);
                    } else {
                        x = maxx + L_CROCODILE * 2;
                    }
                }
            } else if (command.c == BULLET_PROCESS) {
                // Altrimenti se riceve il comando per creare il processo proiettile lo crea
                if (command.x == id){
                    pid_bullet = fork();
                    if (pid_bullet < 0) {
                        perror("fork call");
                        exit(1);
                    } else if (pid_bullet == 0) {
                        bullet(pipe_p, pipe_b);
                        
                    }
                }   
            }
        }

        // Movimento continuo
        if (dir_random == 0) { // Direzione sx → dx
            x += 1;
            if (x > maxx + L_CROCODILE) x = -(L_CROCODILE); // Rientra da sinistra
        } else { // Direzione dx → sx
            x -= 1;
            if (x < -(L_CROCODILE)) x = maxx; // Rientra da destra
        }

        // Invia le coordinate aggiornate al processo control
        struct Coordinates crocodile = {id, x, COORDINATES_CROCODILE, dir_random};
        write(pipe_p[1], &crocodile, sizeof(crocodile));

        usleep(udelay); // Ritardo per il movimento

        if (pid_bullet > 0) { // Se il processo è attivo
            int status;
            pid_t result = waitpid(pid_bullet, &status, WNOHANG); // Controlla senza bloccare
            if (result > 0) { // Il processo è terminato
                pid_bullet = 0; // Rimuovi il pid dall'elenco attivo
            }
        }
    }
}
