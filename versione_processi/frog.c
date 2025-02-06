#include "header.h"

void frog(int pipe_p[], int pipe_f[], int pipe_g[][2]) {
    close(pipe_p[0]);  // Chiude la pipe to parent in lettura
    close(pipe_f[1]);  // Chiude la pipe to frog in scrittura

    // Inizializzazione audio 
    initAudio();
    Mix_Chunk *frogSound = loadSound("audio/frog-2.wav");
    Mix_Chunk *explosionSound = loadSound("audio/robodeth.wav");

    timeout(150); // Attesa 150 millisecondi per l'input

    // Inizializzazione dei pid per le granate
    pid_t pid_grenade[2];

    // Inizializzazione delle struct per le coordinate della rana e generazione delle granate
    struct Coordinates new, generating_grenade;

    // Inizializzazione delle variabili per la grandezza dello schermo e dell'inserimento da input per il movimento
    int maxy = H_PAVEMENT - 2, maxx = (N_DENS * L_FROG) + (4 * L_FROG) + 2, y = 4, x = maxx / 2 - L_FROG / 2, c;

    // Inizializzazione delle variabili per il movimento della rana sul coccodrillo
    int on_crocodile = 0, x_on_crocodile = 0;

    // Inizializzazione della variabile per tenere la rana nei limiti delle varie finestre di gioco
    char last_area = FROG_PAVEMENT;

    while (1) {

        // Legge le coordinate dalla pipe control -> frog per riposizionare la rana nelle finestre e sul coccodrillo 
        if(read(pipe_f[0], &new, sizeof(new)) > 0) {
            if (new.c == FROG_PAVEMENT) {
                y = new.y, x = new.x, maxy = H_PAVEMENT - 2, last_area = FROG_PAVEMENT, on_crocodile = 0;
            }
            else if (new.c == FROG_RIVER) {
                y = new.y, x = new.x, maxy = H_RIVER, last_area = FROG_RIVER, on_crocodile = 1;
            }
            else if (new.c == FROG_GRASS) {
                y = new.y, x = new.x, maxy = H_GRASS, last_area = FROG_GRASS, on_crocodile = 0;
            }
            else if (new.c == FROG_DENS) {
                y = new.y, x = new.x, maxy = H_DENS, last_area = FROG_DENS;
            }
            else if (new.c == FROG_BACK) {
                y = new.y, x = new.x, x_on_crocodile = new.dir; 
                if(last_area == FROG_RIVER) maxy = H_PAVEMENT - 2;
                else if(last_area == FROG_GRASS) {last_area = FROG_RIVER; maxy = H_RIVER;}
                else if(last_area == FROG_DENS) {last_area = FROG_GRASS; maxy = H_GRASS;}
            }
            new.c = FROG_RESET; // Resetta il campo .c della struct per lasciare bloccato il movimento

        // Legge l'inserimento da input dall'altro processo per il movimento tramite socket
        } else if (server(&c) == 1) { 
            switch (c) {
                case KEY_UP:
                    if (y >= 0) y -= 2; // Salta in avanti di due coordinate
                    playSound(frogSound, 0); // Effetto sonoro
                    break;
                case KEY_DOWN:
                    if (y < maxy) y += 2; // Salta indietro di due coordinate
                    playSound(frogSound, 0);
                    break;
                case KEY_LEFT:
                    if (x > 1) x -= 1; // Fa un passo a sinistra
                    if (on_crocodile) x_on_crocodile -= 1; // Se sul coccodrillo fa un passo a sinistra
                    break;
                case KEY_RIGHT:
                    if (x < maxx - L_FROG - 1) x += 1; // Fa un passo a destra
                    if (on_crocodile) x_on_crocodile += 1; // Se sul coccodrillo fa un passo a destra
                    break;
                // Se si preme la barra spaziatrice e la rana è sul coccodrillo genera le coordinate da mandare a control per la granata
                case ' ': 
                    if(on_crocodile) {
                        playSound(explosionSound, 0); // Effetto sonoro
                        generating_grenade.y = y;
                        generating_grenade.c = COORDINATES_FROG_GRENADE;
                        
                        write(pipe_p[1], &generating_grenade, sizeof(generating_grenade));

                        // Genera due processi per le granate
                        for (int i = 0; i < 2; i++) {
                            pid_grenade[i] = fork();
                            if (pid_grenade[i] < 0) {
                                perror("fork call");
                                exit(1);
                            } else if (pid_grenade[i] == 0) {
                                grenade(i, pipe_p, pipe_g); // Passa l'indice del processo
                            }
                        }
                        break;
                    }
            } 
        }
        
        // Invia le coordinate del movimento tramite pipe 
        struct Coordinates frog = {y, x, COORDINATES_FROG, x_on_crocodile};

        write(pipe_p[1], &frog, sizeof(frog));

        for (int i = 0; i < 2; i++) {
            if (pid_grenade[i] > 0) { // Se il processo è attivo
                int status;
                pid_t result = waitpid(pid_grenade[i], &status, WNOHANG); // Controlla senza bloccare
                if (result > 0) { // Il processo è terminato
                    pid_grenade[i] = 0; // Rimuove il pid dall'elenco attivo
                }
            }
        }

    }
    // Libera risorse audio prima di terminare
    freeSound(frogSound);
    freeSound(explosionSound);
    closeAudio();
}
