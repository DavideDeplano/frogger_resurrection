#include "header.h"

void* crocodile(void *arg) {
    // Inizializza il tid per il proiettile
    pthread_t tid_bullet;

    // Inizializza flag per assicurare la creazione di un solo thread
    int thread = 0;
    // Inizializzazione delle struct per il movimento del coccodrillo e per generare il proiettile
    struct Coordinates command;

    // Inizializzazione della variabile per la grandezza dello schermo
    int maxx = (N_DENS * L_FROG) + (4 * L_FROG) + 2; 

    // Inizializzazione per la posizione e la direzione del coccodrillo
    int x, dir_random;                                          

    // Recupera gli elementi passati come parametro
    struct Crocodile_args *args = (struct Crocodile_args *)arg;
    int id = args->id;
    int base = args->udelay;

    // Inizializzazione della variabile per identificare il primo e il secondo coccodrillo della riga nel fiume
    int pair_id = id / 8;                   

    // Inizializzazione della variabile per identificare gli 8 gruppi di due coccodrilli distribuiti sulle 8 righe del fiume       
    int group_id = id % 8;                         

    // Inizializzazione della variabile per regolare la velocità dei coccodrilli con base uguale per tutti e differenziata per gli 8 flussi del fiume
    srand(group_id); // Assicura che ogni gruppo di coccodrilli abbia lo stesso seme di generazione della velocità casuale in modo che non si sovrappongano
    int udelay = base + group_id * rand() % (100000 - 50000) + 50000;

    // Inizializzazione variabili del numero di elementi prodotti o letti 
    int count_generated = 0, count_read = 0;

    // Ciclo principale
    while (1) {
        // Controlla senza bloccare il thread se sono state ricevute le nuove direzioni dei coccodrilli a inizio nuova manche
        if (sem_trywait(&sem_full_crocodile) == 0) {
            while (count_read < 1) {
                command = buffer_crocodile[i_read_crocodile];
                i_read_crocodile = (i_read_crocodile + 1) % DIM_BUFFER_CROCODILE;

                signal_consumer(&sem_empty_crocodile);
                count_read++;
            }
            count_read = 0;
        
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
            } else if (command.c == BULLET_THREAD) {
                // Altrimenti se riceve il comando per creare il thread proiettile lo crea
                thread = 1;
                if (command.x == id && thread){
                    pthread_create(&tid_bullet, NULL, &bullet, NULL);
                    thread = 0;
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
        
        while (count_generated < 1) {
            wait_producer(&sem_empty);
            buffer[i_write] = (struct Coordinates){id, x, COORDINATES_CROCODILE, dir_random};
            i_write = (i_write + 1) % DIM_BUFFER;

            signal_producer(&sem_full);
            count_generated++;
        }
        count_generated = 0;

        usleep(udelay); // Ritardo per il movimento
    }
}




    

