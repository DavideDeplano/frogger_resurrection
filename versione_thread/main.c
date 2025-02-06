#include "header.h"

int main() {
    int play_again = 1; // Variabile per il ciclo principale per la nuova partita
    
    while (play_again) {
        initscr(); // Inizializzazione curses
        start_color(); // Attivazione colori
        noecho(); // Disabilita l'eco dei caratteri inseriti dall'utente
        cbreak(); // Riceve caratteri utente senza premere invio
        keypad(stdscr, 1); // Abilita la lettura di tasti funzione
        curs_set(0); // Nasconde il cursore del terminale
        srand(time(NULL)); // Inizializzazione del seme al tempo attuale

        initAudio(); // Inizializza audio

        // Inizializzazione colori
        init_pair(1, COLOR_WHITE, COLOR_GREEN);
        init_pair(2, COLOR_WHITE, COLOR_RED);
        init_pair(3, COLOR_WHITE, COLOR_CYAN);
        init_color(COLOR_WHITE + 1, 300, 300, 300); // Grigio
        init_pair(4, COLOR_WHITE, COLOR_WHITE + 1);
        init_color(COLOR_WHITE + 2, 300, 150, 0); // Marrone
        init_pair(5, COLOR_WHITE, COLOR_WHITE + 2);

        // Inizializzazione dei tid
        pthread_t tid_frog, tid_crocodile[N_CROCODILE];

        // Inizializzazione pid client
        pid_t pid_client;

        // Inizializzazione variabili per la grandezza dello schermo
        int maxy = H_LIFES_TIME + H_PAVEMENT + H_RIVER + H_GRASS + H_DENS + H_SCORE;
        int maxx = (N_DENS * L_FROG) + (4 * L_FROG) + 2;

        // Inizializzazione variabili per la difficoltà
        int choice, time_limit, time_bonus;
        struct Crocodile_args crocodile_args[N_CROCODILE];

        // Inizializzazione struct per la partita vinta o persa e punteggio finale
        struct Score score;
        score.score = 0;

        // Iniizializzazione del titolo e messaggi di vincita o perdita
        char frogger[6][55] = {
            " ____  ____    ____    _____   _____  ______  ____ ",
            "|  __||  _ \\  /    \\  / ____| / ____||  ____||  _ \\ ",
            "| |__ | |_/ ||  /\\  || /     | /     | |___  | |_/ |",
            "|  __||    / | |  | || | ___ | | ___ |  ___| |    /",
            "| |   | |\\ \\ |  \\/  || |__| || |__| || |____ | |\\ \\ ",
            "|_|   |_| \\_\\ \\____/  \\_____| \\_____||______||_| \\_\\ "};
        char resurration[6][88] = {
            " ____   ______  _____  _     _  ____   ____    _____   ________  __   "
            "_____   __    _",
            "|  _ \\ |  ____|/  ___|| |   | ||  _ \\ |  _ \\  /  _  \\ |__    __|| "
            " | /     \\ |  \\  | |",
            "| |_/ || |___  | |__  | |   | || |_/ || |_/ ||  /_\\  |   |  |   |  "
            "||   _   ||   \\ | |",
            "|    / |  ___|  \\__ \\ | |   | ||    / |    / |   _   |   |  |   |  "
            "||  |_|  || |\\ \\| |",
            "| |\\ \\ | |____  ___| ||  \\_/  || |\\ \\ | |\\ \\ |  / \\  |   |  | "
            "  |  ||       || | \\   |",
            "|_| \\_\\|______||____/  \\_____/ |_| \\_\\|_| \\_\\|_|   |_|   |__|  "
            " |__| \\_____/ |_|  \\__|"};
        char win[7][66] = {
            "__      __  _____   __     __    __          __  __  __    __ ",
            "\\ \\    / / /  _  \\ |  |   |  |  |  |        |  ||  ||  \\  |  | ",
            " \\ \\  / / |  / \\  ||  |   |  |  |  |        |  ||  ||   \\ |  | ",
            "  \\ \\/ /  | |   | ||  |   |  |  |  |   __   |  ||  ||    \\|  | ",
            "   |  |   | |   | ||  |   |  |  |  |  |  |  |  ||  ||  |\\    |", 
            "   |  |   |  \\_/  ||   \\_/   |  |   \\/    \\/   ||  ||  | \\   |", 
            "   |__|    \\_____/  \\_______/    \\_____/\\_____/ |__||__|  \\__|"};
            char lose[7][68] = {
            "__      __  _____   __     __    __       _____    ______  ______",
            "\\ \\    / / /  _  \\ |  |   |  |  |  |     /  _  \\  /  ____||  ____|",
            " \\ \\  / / |  / \\  ||  |   |  |  |  |    |  / \\  ||  |___  | |___",
            "  \\ \\/ /  | |   | ||  |   |  |  |  |    | |   | | \\__   \\ |  ___|",
            "   |  |   | |   | ||  |   |  |  |  |    | |   | |     \\  || |",
            "   |  |   |  \\_/  ||   \\_/   |  |  |___ |  \\_/  | ____/  || |____",
            "   |__|    \\_____/  \\_______/   |______| \\_____/ |______/ |______| "};

        // Finestra iniziale
        refresh();
        WINDOW *start = newwin(maxy, maxx, 0, 0);
        box(start, ACS_VLINE, ACS_HLINE);

        // Stampa delle scritte
        for (int i = 0; i < 6; i++) {
            mvwprintw(start, maxy / 3 + i, (maxx - 54) / 2, "%s", frogger[i]);
        }
        for (int i = 0; i < 6; i++) {
            mvwprintw(start, maxy / 3 + 8 + i, (maxx - 87) / 2, "%s",
                    resurration[i]);
        }
        mvwprintw(start, maxy - (maxy / 4), (maxx - 27) / 2,
                "Press 's' to start the game");
        wrefresh(start);

        if (getch() == (int)'s') { // Se si inserisce 's' il gioco si avvia
            delwin(start);

            clear(); // Pulisce lo schermo prima di visualizzare una nuova finestra

            // Creazione della finestra per la selezione della difficoltà
            WINDOW *difficulty = newwin(maxy, maxx, 0, 0);
            box(difficulty, ACS_VLINE, ACS_HLINE);
            curs_set(0);
            mvwprintw(difficulty, maxy / 2 - 1, (maxx - strlen("Select Difficulty: Easy (e), Medium (m), Hard (h)")) / 2, "Select Difficulty: Easy (e), Medium (m), Hard (h)");
            wrefresh(difficulty); // Aggiorna la finestra per mostrare il testo

            // Loop per gestire la selezione della difficoltà
            while (choice != 'e' && choice != 'm' && choice != 'h') {
                choice = wgetch(difficulty); // Continua a leggere l'input finché non è valido
            }

            switch (choice) {
                case 'e': // Imposta la difficoltà a facile
                    for(int i = 0; i < N_CROCODILE; i++) {
                        crocodile_args[i].udelay = 300000; 
                    }
                    time_limit = 99; time_bonus = 50;
                    break;
                case 'm': // Imposta la difficoltà a media
                    for(int i = 0; i < N_CROCODILE; i++) {
                        crocodile_args[i].udelay = 200000; 
                    }
                    time_limit = 70; time_bonus = 40;
                    break;
                case 'h': // Imposta la difficoltà a difficile
                    for(int i = 0; i < N_CROCODILE; i++) {
                        crocodile_args[i].udelay = 100000; 
                    }
                    time_limit = 50; time_bonus = 30;
                    break;
            }
           
            // Distrugge la finestra di difficoltà dopo la selezione
            delwin(difficulty);
            clear(); // Pulisce lo schermo dopo la chiusura della finestra
           
            // Avvia processo per la tastiera per il movimento della rana
            pid_client = fork();
            if (pid_client < 0) {
                perror("fork call");
                _exit(2);
            } else if (pid_client == 0) {
                execl("/usr/bin/xterm", "xterm", "-e", "./client.out", (char *)NULL); // Avvia automaticamente la schermata
            } 

            // Inizializzazione dei semafori
            init_sync();

            // Creazione del thread frog
            pthread_create(&tid_frog, NULL, &frog, NULL);

            // Creazione dei thread crocodile
            for (int i = 0; i < N_CROCODILE; i++) {
                crocodile_args[i].id = i;
                pthread_create(&tid_crocodile[i], NULL, &crocodile, (void*)&crocodile_args[i]);
            }
            score = control(time_limit, time_bonus); // Funzione controllo

            // Terminazione dei threads
            pthread_tryjoin_np(tid_frog, NULL);
            for(int i = 0; i < N_CROCODILE; i++) {
                pthread_tryjoin_np(tid_crocodile[i], NULL);
            }
            kill(pid_client, SIGTERM); // Invia SIGTERM al client
            waitpid(pid_client, NULL, 0); // Aspetta che il client termini
        }

        // Distruzione dei semafori
        destroy_sync();
       
        // Pulisce lo schermo e mostra il risultato
        clear();
        nodelay(stdscr, 0);
        noecho();
        cbreak();
        curs_set(0);

        refresh();
        WINDOW *end = newwin(maxy, maxx, 0, 0);
        box(end, ACS_VLINE, ACS_HLINE);

        if (score.result == 1) {
            for (int i = 0; i < 7; i++) {
                mvwprintw(end, maxy / 3 + i, (maxx - 66) / 2, "%s", win[i]);
            }
        } else {
            for (int i = 0; i < 7; i++) {
                mvwprintw(end, maxy / 3 + i, (maxx - 68) / 2, "%s", lose[i]);
            }
        }
        mvwprintw(end, (maxy / 3) + 9, (maxx - 8) / 2, "SCORE %2d", score.score); 
        mvwprintw(end, maxy - (maxy / 4), (maxx - 17) / 2, "Play again? (y/n)");
        wrefresh(end);

        // Gestisce l'input per una nuova partita
        int ch = getch();
        if (ch == 'y') {
            play_again = 1; // Ricomincia
        } else {
            play_again = 0; // Esce
        }

        // Elimina la finestra finale
        delwin(end); 
        clear();
        refresh();

        closeAudio(); // Chiude l'audio
        endwin(); // Chiude ncurses
    }
    exit(0);
}

