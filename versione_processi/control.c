#include "header.h"

struct Score control(int pipe_p[], int pipe_f[], int pipe_c[], int pipe_g[][2], int pipe_b[], int time_limit, int time_bonus) {
    close(pipe_p[1]); // Chiude pipe to parent in scrittura
    close(pipe_f[0]); // Chiude pipe to frog in lettura
    close(pipe_c[0]); // Chiude pipe to crocodile in lettura
    close(pipe_g[0][0]); // Chiude la prima matrice pipe to grenade in lettura
    close(pipe_g[1][0]); // Chiude la seconda matrice pipe to grenade
    close(pipe_b[0]); // Chiude pipe to bullet in lettura

    // Inizializzazione audio 
    initAudio();
    Mix_Chunk *splashSound = loadSound("audio/splash_jack_01.wav");
    Mix_Chunk *boomSound = loadSound("audio/roboboom.wav");
    Mix_Chunk *gunSound = loadSound("audio/gun_shot_01.wav");
    Mix_Chunk *dethSound = loadSound("audio/fratdeth.wav");
    Mix_Chunk *powerupSound = loadSound("audio/powerup_01.wav");
    Mix_Chunk *painSound = loadSound("audio/pain_jack_01.wav");

    // Struct per l'invio di coordinate e direzioni tra processo padre e processi figli
    struct Coordinates temp, new;

    // Variabili grafica rana, coccodrillo, vite
    char frog[H_FROG][L_FROG + 1] = {" |\\ @..@ /|", "_| \\|  |/ |_"};
    char crocodile_left[H_FROG][L_CROCODILE + 1] =
                                  {"<<<<<|======|====|o\\\\\\\\\\",
                                   "<<<<<|======|====|o/////"};
    char crocodile_right[H_FROG][L_CROCODILE + 1] =
                                   {"/////o|====|======|>>>>>",
                                    "\\\\\\\\\\o|====|======|>>>>>"};
    char lifes[5] = {'*', '*', '*', '*', '*'};

    // Variabili grandezza schermo
    int maxy = H_LIFES_TIME + H_PAVEMENT + H_RIVER + H_GRASS + H_DENS + H_SCORE;
    int maxx = (N_DENS * L_FROG) + (4 * L_FROG) + 2; 
    int corner = 0;

    // Variabili coordinate rana
    int y_pavement_grass_start = 4, x_start = maxx / 2 - L_FROG / 2, y_frog = y_pavement_grass_start, x_frog = x_start, x_relative_on_crocodile = 0;
    int y_river_start = 14, y_dens_start = 10, y_den = 2; 

    // Variabili coordinate coccodrillo
    int y_crocodile[N_CROCODILE], x_crocodile[N_CROCODILE], x_crocodile_start[N_CROCODILE], x_crocodile_end[N_CROCODILE], direction_crocodile[N_CROCODILE], dir_random;

    // Variabili di supporto per il coccodrillo 
    int crocodile_1, crocodile_2, i_crocodile, reference_crocodile, x_on_crocodile;
    int on_crocodile = 0, last_x_on_crocodile = -1, relative_on_crocodile = -1;
    
    // Variabili per le granate della rana
    int grenade = 0, y_grenade, x_grenade_left, x_grenade_right;

    // Variabili per i proiettili dei coccodrilli
    int bullet = 0, y_bullet, x_bullet, direction_bullet, i_crocodile_bullet, interval = (rand() % (10 - 5 + 1)) + 5;
    time_t bullet_timer;

    // Variabili gestione delle partite, vite, tempo
    int new_match = 1, tot_score = 0, match = 0, count_lifes = 5, first_check = 1;
    time_t start_time = time(NULL), last_update = start_time, current_time;
    int time_remaining = time_limit, time_expired = 0;
    struct Score score;

    // Variabili per le tane
    int den_visited[N_DENS] = {0, 0, 0, 0, 0}, h_den = H_FROG * 2, count_dens_visited = 0;
    int x_den[N_DENS], x_den_start = 1, in_any_den;
    for (int i = 0; i < N_DENS; i++) {
        x_den[i] = x_den_start;
        x_den_start += L_FROG * 2;
    }

    // Variabili per l'attivazione delle finestre al passaggio della rana
    int in_pavement = 1, in_river = 0, in_grass = 0, in_dens = 0;

    // Variabili delle mine
    int y_mine[N_MINE] = {2, 2, 2, 2, 2}, x_mine[N_MINE];
    
    // Inizializzazione delle finestre
    WINDOW *win_score = newwin(H_SCORE, maxx, corner, 0); // Finestra punteggio
    WINDOW *win_dens = newwin(H_DENS, maxx, corner += H_SCORE, 0); // Finestra tane
    WINDOW *win_grass = newwin(H_GRASS, maxx, corner += H_DENS, 0); // Finestra prato
    WINDOW *win_river = newwin(H_RIVER, maxx, corner += H_GRASS, 0); // Finestra fiume
    WINDOW *win_pavement = newwin(H_PAVEMENT, maxx, corner += H_RIVER, 0); // Finestra marciapiede
    WINDOW *win_lifes_time = newwin(H_LIFES_TIME, maxx, corner += H_PAVEMENT, 0); // Finestra vite e tempo di gioco
    WINDOW *win_den[N_DENS]; // Finestre tane
    WINDOW *win_between_dens[N_DENS - 1]; // Finestre tra le tane

    // Assegnamento dei colori e bordi alle finestre
    box(win_score, ACS_VLINE, ACS_HLINE);
    wbkgd(win_dens, COLOR_PAIR(5));
    wbkgd(win_grass, COLOR_PAIR(1));
    wbkgd(win_river, COLOR_PAIR(3));
    wbkgd(win_pavement, COLOR_PAIR(4));
    box(win_lifes_time, ACS_VLINE, ACS_HLINE);

    // Stampa grafica punteggio, vite, tempo
    mvwprintw(win_score, 1, 1, "SCORE: %d", tot_score);
    mvwprintw(win_lifes_time, 1, 1, "LIFES: %s", lifes);
    mvwprintw(win_lifes_time, 1, maxx - 9, "TIME: %d", time_remaining);

    // Stampa grafica rana
    for (int i = 0; i < 2; i++) {
        mvwprintw(win_pavement, y_frog + i, x_frog, "%s", frog[i]);
    }
    
    // Assegnamento del colore verde a tutte le tane libere a inizio partita
    for (int i = 0; i < N_DENS; i++) {
        win_den[i] = subwin(win_dens, h_den, L_FROG, 4, x_den[i]);
        wbkgd(win_den[i], COLOR_PAIR(1));
        wrefresh(win_den[i]);
    }

    // Assegnamento del colore rosso tra le tane, se la rana ci passa sopra perde una vita
    for(int i = 0; i < N_DENS - 1; i++) {
        win_between_dens[i] = subwin(win_dens, h_den, L_FROG, 4, x_den[i] + L_FROG);
        wbkgd(win_between_dens[i], COLOR_PAIR(2));
        wrefresh(win_between_dens[i]);
    }
    
    // Aggiornamento delle finestre
    wrefresh(win_score);
    wrefresh(win_dens);
    wrefresh(win_grass);
    wrefresh(win_river);
    wrefresh(win_pavement);
    wrefresh(win_lifes_time);

    while (count_lifes > 0 && count_dens_visited != N_DENS) { // Inizia il ciclo della partita, finchè ci sono vite disponibili o non sono state visitate tutte le tane
        current_time = time(NULL); // Aggiorna il tempo corrente
        time_remaining = time_limit - (int)(current_time - start_time); // Calcola il tempo rimanente

        if (new_match) { // Se è un nuovo match
            start_time = time(NULL); // Aggiorna il tempo di inizio
            time_remaining = time_limit; // Aggiorna il tempo rimanente
            bullet_timer = time(NULL); // Inizializza il timer per i proiettili

            // Genera posizioni casuali per le mine nel prato
            for (int i = 0; i < N_MINE; i++) {
                x_mine[i] = rand() % maxx;
            }

            // Genera direzioni casuali per i coccodrilli e le invia attraverso la pipe
            dir_random = rand() % 2;
            for (int i = 0; i < N_CROCODILE; i++) {
                new.dir = dir_random;
                new.c = CROCODILE_DIRECTION; 
                write(pipe_c[1], &new, sizeof(new));
            }

            // Se ci sono granate o proiettili in gioco manda un segnale di terminazione dei relativi processi con le coordinate fuori schermo
            if (x_grenade_left < maxx) {
                new.x = maxx;
                new.c = TERMINATE_GRENADE;
                new.dir = direction_bullet;
                write(pipe_g[0][1], &new, sizeof(new));
            }

            if (x_grenade_left > 0) {
                new.x = -1;
                new.c = TERMINATE_GRENADE;
                new.dir = direction_bullet;
                write(pipe_g[1][1], &new, sizeof(new));
            }

            if (x_bullet > 0 && x_bullet < maxx) {
                new.x = maxx;
                new.c = TERMINATE_BULLET;
                write(pipe_b[1], &new, sizeof(new));
            }

            new_match = 0; // Resetta la variabile per il nuovo match
        }

        // Lettura delle coordinate nella pipe
        if (read(pipe_p[0], &temp, sizeof(temp)) > 0) { // Se ci sono coordinate nella pipe le legge
            if (temp.c == COORDINATES_FROG) { // Se le coordinate sono della rana le assegna
                y_frog = temp.y;
                x_frog = temp.x;
                x_relative_on_crocodile = temp.dir; // Assegna la posizione relativa della rana sul coccodrillo in caso si muova su di esso
            } else if (temp.c == COORDINATES_CROCODILE) { // Se le coordinate sono dei coccodrilli le assegna
                if(temp.y < 8) { // temp.y contiene l'indice del coccodrillo, se è minore di 8
                    y_crocodile[temp.y] = temp.y * 2; // In base all'indice determina la posizione verticale
                    x_crocodile[temp.y] = temp.x;
                    direction_crocodile[temp.y] = temp.dir; // Salva la direzione del coccodrillo
                } else { // Altrimenti se è maggiore di 8
                    reference_crocodile = temp.y - 8;  // Determina il coccoodrillo di riferimento nel gruppo per determinare primo e secondo coccodrillo nel flusso
                    y_crocodile[temp.y] = y_crocodile[reference_crocodile];  // Posizione verticale basata sul coccodrillo di riferimento
                    x_crocodile[temp.y] = temp.x;  // Usa direttamente la posizione x ricevuta
                    direction_crocodile[temp.y] = temp.dir;  // Salva la direzione del coccodrillo
                }
            } else if (temp.c == COORDINATES_FROG_GRENADE) { // Coordinata y mandata da frog quando viene premuta la barra spaziatrice
                y_grenade = temp.y; // Assegna alle granate la posizione y della rana
            } else if(temp.c == GRENADE_CREATED) { // Se la granata è stata creata
                grenade += 1; // Incrementa il contatore delle granate
            } else if (temp.c == BULLET_CREATED) { // Se il proiettile è stato creato
                bullet = 1; // Assegna il valore 1 alla variabile bullet
            } else if(temp.c == COORDINATES_X_GRENADE_LEFT) { // Se la coordinata è per la granata sinistra la assegna
                x_grenade_left = temp.x;
            } else if(temp.c == COORDINATES_X_GRENADE_RIGHT) { // Se la coordinata è per la granata destra la assegna
                x_grenade_right = temp.x;
            } else if (temp.c == COORDINATES_BULLET) { // Se le coordinate sono per il proiettile le assegna
                y_bullet = temp.y;
                x_bullet = temp.x;
                direction_bullet = temp.dir; // Assegna la direzione del proiettile
            } 
        }
        
        if(grenade == 2) { // Quando entrambi i processi granata sono stati creati e avviati invia le coordinate della granata attraverso la matrice di pipe per differenziarle
            for(int i = 0; i < 2; i++) {
                new.x = x_frog;
                new.c = COORDINATE_X_FROG_GRENADE;
                new.dir = i;
                write(pipe_g[i][1], &new, sizeof(new)); 
            }
            grenade = 0; // Resetta il contatore delle granate
        }

        // Pulisce le finestre dinamiche
        werase(win_river);
        werase(win_grass);
        
        // Stampa dei coccodrilli
        for (int i = 0; i < N_CROCODILE; i++) {
            for (int j = 0; j < 2; j++) { // Itera sulle righe dello sprite
                for (int k = 0; k < L_CROCODILE; k++) { // Itera sui caratteri dello sprite
                    if (direction_crocodile[i] == 0) {
                        // Coccodrillo che si muove da sinistra a destra
                        mvwaddch(win_river, y_crocodile[i] + j, x_crocodile[i] + k, crocodile_left[j][k]);
                    } else {
                        // Coccodrillo che si muove da destra a sinistra
                        mvwaddch(win_river, y_crocodile[i] + j, x_crocodile[i] + k, crocodile_right[j][k]);
                    }
                }
            }
        }

        // Stampa delle mine
        for(int i = 0; i < N_MINE; i++) {
                mvwprintw(win_grass, y_mine[i], x_mine[i], "X");
            }
        
        // Movimento rana sul marciapiede
        if (in_pavement) { // Se la rana è sul marciapiede
            in_grass = 0; // Disattiva il prato
            in_dens = 0; // Disattiva le tane
            
            werase(win_pavement); // Pulisce la finestra del marciapiede

            // Stampa la rana sul marciapiede
            for (int i = 0; i < H_FROG; i++) {
                mvwprintw(win_pavement, y_frog + i, x_frog, "%s", frog[i]);
            }

            if(y_frog == -2) { // Se la rana è alla fine del marciapiede
                in_pavement = 0; // Disattiva il marciapiede
                in_river = 1; // Attiva il fiume
                in_grass = 0; // Disattiva il prato
                
                // Invia le coordinate a frog per aggiornare le coordinate della rana, dato che non utilizzo le coordinate assolute ma relative alle finestre
                new.y = y_river_start;
                new.x = x_frog;
                new.c = FROG_RIVER;
                write(pipe_f[1], &new, sizeof(new));
                wrefresh(win_river);
            }
            wrefresh(win_pavement); // Aggiorna la finestra del marciapiede
        }

        // Movimento rana nel fiume
        if (in_river && !in_pavement) { // Se la rana è nel fiume e non sul marciapiede

            // Se le granate sono in gioco le stampa
            if (x_grenade_left > 0 && x_grenade_left < maxx ||
                x_grenade_right > 0 && x_grenade_right < maxx) {
                mvwaddch(win_river, y_grenade, x_grenade_left, 'o');
                mvwaddch(win_river, y_grenade, x_grenade_right, 'o');
            }

            // Calcolo delle coordinate di inizio e fine per ogni coccodrillo
            for (int i = 0; i < N_CROCODILE; i++) {
                x_crocodile_start[i] = x_crocodile[i];
                x_crocodile_end[i] = x_crocodile[i] + L_CROCODILE - 1;
            }

            // Controlla la y della rana per la collisione con i coccoodrilli
            if (y_frog >= 0 && y_frog <= y_river_start) {
                crocodile_1 = y_frog / 2; // Identifica l'indice del primo coccodrillo nel flusso in base alla y
                crocodile_2 = crocodile_1 + N_CROCODILE / 2; // E quello del secondo coccodrillo aggiungendo 8

                // Verifica collisione con il primo coccodrillo
                if (!on_crocodile && x_frog >= x_crocodile_start[crocodile_1] && (x_frog + L_FROG - 1) <= x_crocodile_end[crocodile_1]) {
                    on_crocodile = 1; // Collisione rilevata
                    i_crocodile = crocodile_1; // Salva l'indice del coccodrillo

                    // Calcola la posizione relativa sul coccodrillo
                    if (relative_on_crocodile == -1) {
                        relative_on_crocodile = x_frog - x_crocodile_start[i_crocodile];
                    }
                }
                // Verifica collisione con il secondo coccodrillo
                else if (!on_crocodile && x_frog >= x_crocodile_start[crocodile_2] && (x_frog + L_FROG - 1) <= x_crocodile_end[crocodile_2]) {
                    on_crocodile = 1; // Collisione rilevata
                    i_crocodile = crocodile_2; // Salva l'indice del coccodrillo
                   
                    // Calcola la posizione relativa sul coccodrillo
                    if (relative_on_crocodile == -1) {
                        relative_on_crocodile = x_frog - x_crocodile_start[i_crocodile];
                    }
                } else if (!on_crocodile) { // Altrimenti se non collide
                    // Resetta tutte le variabili relative al coccodrillo
                    relative_on_crocodile = -1;
                    x_relative_on_crocodile = 0;

                    // Aggiorna i flag delle finestre
                    in_pavement = 1;
                    in_river = 0;
                    in_grass = 0;
                    in_dens = 0;

                    // Decrementa le vite
                    count_lifes--;
                    lifes[count_lifes] = ' '; // Rimuove la vita graficamente

                    playSound(splashSound, 0); // Effetto sonoro 

                    // Invia le coordinate a frog per aggiornare le coordinate della rana
                    new.y = y_pavement_grass_start;
                    new.x = x_start;
                    new.c = FROG_PAVEMENT;
                    new.dir = x_relative_on_crocodile;
                    write(pipe_f[1], &new, sizeof(new));

                    // Pulice e aggiorna la finestra delle vite
                    werase(win_lifes_time);
                    box(win_lifes_time, ACS_VLINE, ACS_HLINE);
                    mvwprintw(win_lifes_time, 1, 1, "LIFES: %s", lifes);
                    wrefresh(win_lifes_time);

                    new_match = 1; // Nuova partita
                }
            }

            if (x_frog == 0) { // Se la rana è sopra il coccodrillo e arriva al bordo sinistro dello schermo
                relative_on_crocodile++; // La sposta verso destra 
                if(x_frog + L_FROG > x_crocodile_end[i_crocodile]) on_crocodile = 0; // Se la rana non è più sopra il coccodrillo cade
            }

            if (x_frog + L_FROG == maxx) { // Se la rana è sopra il coccodrillo e arriva al bordo destro dello schermo
                relative_on_crocodile--; // La sposta verso sinistra
                if(x_frog < x_crocodile_start[i_crocodile]) on_crocodile = 0; // Se la rana non è più sopra il coccodrillo cade
            }

            if (on_crocodile) { // Se la rana è sopra il coccodrillo
                // Segue il movimento del coccodrillo, mantiene la posizione relativa sul coccodrillo, si può muovere con i tasti direzionali su di esso
                x_frog = x_crocodile_start[i_crocodile] + relative_on_crocodile + x_relative_on_crocodile;
            } 

            // Controlla che la rana sia effettivamente sopra il coccodrillo, se non lo è resetta le variabili
            if (on_crocodile && (x_frog < x_crocodile_start[i_crocodile] || (x_frog + L_FROG - 1) > x_crocodile_end[i_crocodile])) {
                on_crocodile = 0;
                relative_on_crocodile = -1;
                x_relative_on_crocodile = 0;
                new_match = 1;
            }

            if (!on_crocodile || y_frog != y_crocodile[i_crocodile]) { // Se la rana si sposta da un coccodrillo all'altro resetta le variabili
                x_relative_on_crocodile = 0;
                on_crocodile = 0;
                relative_on_crocodile = -1;
            }
            
            // Generazione del proiettile del coccodrillo
            if (on_crocodile ) { // Se la rana è sopra il coccodrillo
                if (i_crocodile < 8) { // Se il coccodrillo è il primo del flusso
                    i_crocodile_bullet = i_crocodile +  N_CROCODILE / 2; // Il coccodrillo che spara è il secondo
                } else {
                    i_crocodile_bullet = i_crocodile - N_CROCODILE / 2; // Altrimenti è il primo
                }
        
                if ((int)(current_time - bullet_timer) >= interval) { // Se è passato un intervallo di tempo tra i 5 e i 10 secondi
                    bullet_timer = current_time; // Aggiorna il timer del proiettile
                    interval = (rand() % (10 - 5 + 1)) + 5; // Genera un nuovo intervallo casuale tra 5 e 10 secondi

                    // Controlla se il coccodrillo che deve sparare è nella grafica di gioco
                    if ((direction_crocodile[i_crocodile_bullet] == 0 && x_crocodile_end[i_crocodile_bullet] > 0) ||
                        (direction_crocodile[i_crocodile_bullet] == 1 && x_crocodile_start[i_crocodile_bullet] < maxx)) {

                        // Invia ad ogni coccodrillo il comando di generare il processo proiettile
                        for (int i = 0; i < N_CROCODILE; i++) {
                            new.x = i_crocodile_bullet; // Lo genera solo il coccodrillo che deve sparare
                            new.c = BULLET_PROCESS;
                            
                            write(pipe_c[1], &new, sizeof(new));
                        }

                        if (bullet) { // Se il processo proiettile è stato creato ed è attivo invia le coordinate di partenza
                            new.y = y_frog;
                            if (direction_crocodile[i_crocodile_bullet] == 0) {
                                new.x = x_crocodile_end[i_crocodile_bullet];
                            } else {
                                new.x = x_crocodile_start[i_crocodile_bullet];
                            }
                            new.c = COORDINATE_X_CROCODILE_BULLET;
                            new.dir = direction_crocodile[i_crocodile_bullet];
                            write(pipe_b[1], &new, sizeof(new));

                            playSound(gunSound, 0); // Effetto sonoro
                        }

                        bullet = 0; // Resetta il flag proiettile
                        i_crocodile_bullet = -1; // Resetta l'indice del coccodrillo che deve sparare
                    }
                }   
            }

            // In base alla direzione stampa il proiettile
            if (direction_bullet == 0) {
                mvwaddch(win_river, y_bullet, x_bullet, '>');
            } else {
                mvwaddch(win_river, y_bullet, x_bullet, '<');
            } 

            // Stampa della rana nel fiume
            for (int j = 0; j < H_FROG; j++) {
                mvwprintw(win_river, y_frog + j, x_frog, "%s", frog[j]);
            }

            // Controlla la collisione tra la rana e il proiettile, se collidono la rana perde una vita e ricomincia la nuova manche
            if (y_bullet == y_frog && (x_bullet == x_frog || x_bullet == x_frog + L_FROG - 1)) {
                in_pavement = 1;
                in_river = 0;
                in_grass = 0;
                on_crocodile = 0;
                relative_on_crocodile = -1;
                x_relative_on_crocodile = 0;

                count_lifes--;
                lifes[count_lifes] = ' ';

                playSound(dethSound, 0);

                new.y = y_pavement_grass_start;
                new.x = x_start;
                new.c = FROG_PAVEMENT;
                new.dir = x_relative_on_crocodile;
                write(pipe_f[1], &new, sizeof(new));

                // Aggiorna vite
                werase(win_lifes_time);
                box(win_lifes_time, ACS_VLINE, ACS_HLINE);
                mvwprintw(win_lifes_time, 1, 1, "LIFES: %s", lifes);
                wrefresh(win_lifes_time);

                new_match = 1;

            }

            // Controlla la collisione tra proiettile e granate, se collidono invia le coordinate di terminazione per entrambi i processi
            if (y_grenade == y_bullet) {
                if (x_grenade_left == x_bullet) {
                    x_grenade_left = maxx;
                    new.x = maxx;
                    new.c = TERMINATE_GRENADE;
                    new.dir = direction_bullet;
                    write(pipe_g[0][1], &new, sizeof(new));

                    new.x = -1;
                    new.c = TERMINATE_BULLET;
                    write(pipe_b[1], &new, sizeof(new));

                    playSound(boomSound, 0);

                } else if (x_grenade_right == x_bullet) {
                    x_grenade_right = -1;
                    new.x = -1;
                    new.c = TERMINATE_GRENADE;
                    new.dir = direction_bullet;
                    write(pipe_g[1][1], &new, sizeof(new));

                    new.x = maxx;
                    new.c = TERMINATE_BULLET;
                    write(pipe_b[1], &new, sizeof(new));

                    playSound(boomSound, 0);
                }
            }

            // Se la rana è sul bordo inferiore del fiume a -2 permette di tornare nel marciapiede
            if (y_frog == y_river_start + 2) {  
                on_crocodile = 0;
                in_river = 0;     
                in_pavement = 1;  
                in_grass = 0;
                in_dens = 0;
                x_relative_on_crocodile = 0;

                new.y = 0;
                new.x = x_frog;
                new.c = FROG_BACK;
                new.dir = x_relative_on_crocodile;
                write(pipe_f[1], &new, sizeof(new));
            }

            if (y_frog == 0) in_grass = 1; // Se è alla fine della finestra del fiume attiva il prato

        // Movimento rana sul prato
            if (y_frog == -2 && in_grass && in_river) { 
                in_river = 0;
                in_grass = 1;

                new.y = y_pavement_grass_start;
                new.x = x_frog;
                new.c = FROG_GRASS;
                write(pipe_f[1], &new, sizeof(new));
            }
        }

        if (in_grass && !in_river) { // Se la rana è sul prato e non nel fiume
            werase(win_grass); // Pulisce la finestra del prato

            // Stampa le mine sul prato
            for(int i = 0; i < 5; i++) {
                mvwprintw(win_grass, y_mine[i], x_mine[i], "X");
            }

            // Controlla la collisione tra la rana e le mine, se collidono la rana perde una vita e ricomincia la nuova manche
            for(int i = 0; i < N_MINE; i++) {
                if (y_frog == y_mine[i] && (x_frog + L_FROG - 1 >= x_mine[i] && x_frog <= x_mine[i])) {
                    in_pavement = 1;
                    in_river = 0;
                    in_grass = 0;
                    on_crocodile = 0;
                    relative_on_crocodile = -1;
                    x_relative_on_crocodile = 0;

                    count_lifes--;
                    lifes[count_lifes] = ' ';

                    playSound(painSound, 0);

                    new.y = y_pavement_grass_start;
                    new.x = x_start;
                    new.c = FROG_PAVEMENT;
                    new.dir = x_relative_on_crocodile;
                    write(pipe_f[1], &new, sizeof(new));

                    // Aggiorna vite
                    werase(win_lifes_time);
                    box(win_lifes_time, ACS_VLINE, ACS_HLINE);
                    mvwprintw(win_lifes_time, 1, 1, "LIFES: %s", lifes);
                    wrefresh(win_lifes_time);

                    new_match = 1;
                }
            }

            // Stampa la rana sul prato
            for (int i = 0; i < H_FROG; i++) {
                mvwprintw(win_grass, y_frog + i, x_frog, "%s", frog[i]);
            }
            
            if (y_frog == 0) in_dens = 1; // Se la rana è alla fine del prato attiva le tane

            if (y_frog == y_pavement_grass_start + 2) {  // Se la rana è all'inizio del prato permette di tornare nel fiume
                in_pavement = 0;
                in_river = 1;     
                in_grass = 0; 
                in_dens = 0;

                new.y = 0;
                new.x = x_frog;
                new.c = FROG_BACK;
                write(pipe_f[1], &new, sizeof(new));
            }
            wrefresh(win_grass);
        }

        // Movimento rana sulla zona delle tane
        if (y_frog == -2 && in_dens && in_grass) {
            in_grass = 0;
            in_dens = 1;

            new.y = y_dens_start;
            new.x = x_frog;
            new.c = FROG_DENS;
            write(pipe_f[1], &new, sizeof(new));
        }

        if (in_dens && !in_grass) { // Se la rana è sulle tane e non sul prato
            werase(win_dens);

            // Stampa delle tane
            for (int i = 0; i < N_DENS; i++) {
                win_den[i] = subwin(win_dens, h_den, L_FROG, 4, x_den[i]);
                if (!den_visited[i]) // Se la tanà non è stata visitata
                    wbkgd(win_den[i], COLOR_PAIR(1)); // Assegna il colore verde
                else // Altrimenti
                    wbkgd(win_den[i], COLOR_PAIR(2)); // Assegna il colore rosso
                wrefresh(win_den[i]);
            }

            // Stampa le finestre tra le tane colorandole di rosso
            for(int i = 0; i < N_DENS - 1; i++) {
                win_between_dens[i] = subwin(win_dens, h_den, L_FROG, 4, x_den[i] + L_FROG);
                wbkgd(win_between_dens[i], COLOR_PAIR(2));
                wrefresh(win_between_dens[i]);
            }

            // Stampa la rana sulle tane
            for (int i = 0; i < H_FROG; i++) {
                mvwprintw(win_dens, y_frog + i, x_frog, "%s", frog[i]);
            }

            // Permette di tornare indietro sul prato
            if (y_frog == y_dens_start + 2) {  
                in_pavement = 0;
                in_river = 0;
                in_grass = 1;     
                in_dens = 0;  

                new.y = 0;
                new.x = x_frog;
                new.c = FROG_BACK;
                write(pipe_f[1], &new, sizeof(new));
            }

            in_any_den = 0;  // Flag per verificare se la rana è in una tana qualsiasi
 
            // Scansione delle tane
            for (int i = 0; i < N_DENS; i++) {
                if (y_frog == 2 && in_dens && x_frog == x_den[i]) { // Controlla se è in una coordinata valida di una delle tane
                    in_any_den = 1; // Attiva il flag
 
                    if (!den_visited[i] && first_check) { // Se la rana non è stata visitata 
                        first_check = 0; // Il flag evita di decrementare più volte le vite per la stessa tana, perchè sto usando il ciclo for e può verificarsi più volte la condizione
                        if (time_remaining > time_bonus) { // Se il tempo rimanente è maggiore di 30 secondi
                            tot_score += 10; // Incrementa il punteggio di 10
                        } else { // Altrimenti
                            tot_score += 5; // Incrementa il punteggio di 5
                        }
                        den_visited[i] = 1; // Imposta la tana come visitata
                        count_dens_visited++; // Aggiorna la variabile del conteggio di tane visitate
                        new_match = 1; // Nuova partita

                        // Aggiorna punteggio
                        werase(win_score);
                        box(win_score, ACS_VLINE, ACS_HLINE);
                        mvwprintw(win_score, 1, 1, "SCORE: %d", tot_score);
                        wrefresh(win_score);

                        werase(win_dens);
                        for (int i = 0; i < N_DENS; i++) {
                            win_den[i] = subwin(win_dens, h_den, L_FROG, 4, x_den[i]);
                            if (!den_visited[i])
                                wbkgd(win_den[i], COLOR_PAIR(1));
                            else
                            wbkgd(win_den[i], COLOR_PAIR(2));
                            wrefresh(win_den[i]);
                        }

                        for(int i = 0; i < N_DENS - 1; i++) {
                            win_between_dens[i] = subwin(win_dens, h_den, L_FROG, 4, x_den[i] + L_FROG);
                            wbkgd(win_between_dens[i], COLOR_PAIR(2));
                            wrefresh(win_between_dens[i]);
                        }

                        wrefresh(win_pavement);

                        playSound(powerupSound, 0);

                        break;  // Esce dal ciclo
                    } else if (den_visited[i] && first_check) { // Se la rana è già stata visitata perde la vita e inizia una nuova manche
                        first_check = 0;
                        count_lifes--;
                        lifes[count_lifes] = ' ';
                        new_match = 1;

                        // Aggiorna vite
                        werase(win_lifes_time);
                        box(win_lifes_time, ACS_VLINE, ACS_HLINE);
                        mvwprintw(win_lifes_time, 1, 1, "LIFES: %s", lifes);
                        wrefresh(win_lifes_time);

                        werase(win_dens);
                        for (int i = 0; i < N_DENS; i++) {
                            win_den[i] = subwin(win_dens, h_den, L_FROG, 4, x_den[i]);
                            if (!den_visited[i])
                                wbkgd(win_den[i], COLOR_PAIR(1));
                            else
                            wbkgd(win_den[i], COLOR_PAIR(2));
                            wrefresh(win_den[i]);
                        }

                        for(int i = 0; i < N_DENS - 1; i++) {
                            win_between_dens[i] = subwin(win_dens, h_den, L_FROG, 4, x_den[i] + L_FROG);
                            wbkgd(win_between_dens[i], COLOR_PAIR(2));
                            wrefresh(win_between_dens[i]);
                        }

                        playSound(dethSound, 0);

                        break;  // Esce dal ciclo
                    }
                }
            }

            if (!in_any_den && y_frog == 2 && in_dens && first_check) { // Se la rana non entra perfettamente nelle tane e va in una posizione non valida perde una vita
                first_check = 0;  
                count_lifes--;
                lifes[count_lifes] = ' ';
                new_match = 1;

                // Aggiorna vite
                werase(win_lifes_time);
                box(win_lifes_time, ACS_VLINE, ACS_HLINE);
                mvwprintw(win_lifes_time, 1, 1, "LIFES: %s", lifes);
                wrefresh(win_lifes_time);

                werase(win_dens);
                for (int i = 0; i < N_DENS; i++) {
                    if (!den_visited[i])
                        wbkgd(win_den[i], COLOR_PAIR(1));
                    else
                    wbkgd(win_den[i], COLOR_PAIR(2));
                    wrefresh(win_den[i]);
                }

                for(int i = 0; i < N_DENS - 1; i++) {
                    win_between_dens[i] = subwin(win_dens, h_den, L_FROG, 4, x_den[i] + L_FROG);
                    wbkgd(win_between_dens[i], COLOR_PAIR(2));
                    wrefresh(win_between_dens[i]);
                }

                playSound(dethSound, 0);

            }

            if (in_any_den == 0 && y_frog != 4 && in_dens) first_check = 1; // Resetta il flag nel caso in cui la rana non sia in una tana o non sia in una posizione valida

            // Se la rana entra in una tana la riposiziona sul marciapiede per la partita successiva
            if (y_frog == y_den && in_dens) { 
                in_dens = 0;
                in_pavement = 1;

                new.y = y_pavement_grass_start;
                new.x = x_start;
                new.c = FROG_PAVEMENT;
                write(pipe_f[1], &new, sizeof(new));  
            }
            wrefresh(win_dens);
        }
        
        // Aggiorna graficamente il timer
        werase(win_lifes_time);
        box(win_lifes_time, ACS_VLINE, ACS_HLINE);
        mvwprintw(win_lifes_time, 1, 1, "LIFES: %s", lifes);
        mvwprintw(win_lifes_time, 1, maxx - 9, "TIME: %02d", time_remaining);
            
        // Nuova manche se il rempo è scaduto
        if (time_remaining == 0) {
            if (in_dens) { // Unica finestra che viene ripulita perchè non ha una grafica dinamica come river e grass
                werase(win_dens);
                for (int i = 0; i < N_DENS; i++) {
                    win_den[i] = subwin(win_dens, h_den, L_FROG, 4, x_den[i]);
                    if (!den_visited[i])
                        wbkgd(win_den[i], COLOR_PAIR(1));
                    else
                    wbkgd(win_den[i], COLOR_PAIR(2));
                    wrefresh(win_den[i]);
                }

                for(int i = 0; i < N_DENS - 1; i++) {
                    win_between_dens[i] = subwin(win_dens, h_den, L_FROG, 4, x_den[i] + L_FROG);
                    wbkgd(win_between_dens[i], COLOR_PAIR(2));
                    wrefresh(win_between_dens[i]);
                }
                wrefresh(win_dens);
            }

            count_lifes--;
            lifes[count_lifes] = ' ';

            in_pavement = 1;
            in_river = 0;
            on_crocodile = 0;
            in_grass = 0;
            in_dens = 0;

            new.y = y_pavement_grass_start;
            new.x = x_start;
            new.c = FROG_PAVEMENT;
            write(pipe_f[1], &new, sizeof(new));

            playSound(dethSound, 0);

            new_match = 1;
        }

        // Aggiorna le finestre dinamiche
        wrefresh(win_river);
        wrefresh(win_grass);
        wrefresh(win_lifes_time);

    }

    // Distrugge tutte le finestre
    for (int i = 0; i < N_DENS; i++) {
        delwin(win_den[i]);
    }
    for (int i = 0; i < N_DENS - 1; i++) {
        delwin(win_between_dens[i]);
    }
    delwin(win_score);
    delwin(win_dens);
    delwin(win_grass);
    delwin(win_river);
    delwin(win_pavement);
    delwin(win_lifes_time);
    refresh();

    // Chiude l'audio e libera le risorse
    freeSound(splashSound);
    freeSound(boomSound);
    freeSound(gunSound);
    freeSound(dethSound);
    freeSound(powerupSound);
    closeAudio();

    if (count_dens_visited == N_DENS) { // Se tutte le tane sono state visitate
        score.result = 1; // La partita è vinta
    } else {
        score.result = 0; // Altrimenti persa
    }
    score.score = tot_score; 
    return score; // Restituisce la struct di vincita/perdita e il punteggio
}


