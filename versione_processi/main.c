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

        // Inizializzazione colori
        init_pair(1, COLOR_WHITE, COLOR_GREEN);
        init_pair(2, COLOR_WHITE, COLOR_RED);
        init_pair(3, COLOR_WHITE, COLOR_CYAN);
        init_color(COLOR_WHITE + 1, 300, 300, 300); // Grigio
        init_pair(4, COLOR_WHITE, COLOR_WHITE + 1);
        init_color(COLOR_WHITE + 2, 300, 150, 0); // Marrone
        init_pair(5, COLOR_WHITE, COLOR_WHITE + 2);

        // Inizializzazione dei pid
        pid_t pid_frog, pid_crocodile[N_CROCODILE], pid_client;

        // Inizializzazione delle pipes
        int pipe_to_parent[2], pipe_to_frog[2], pipe_to_crocodile[2], pipe_to_grenade[2][2], pipe_to_bullet[2];

        // Inizializzazione variabili per la grandezza dello schermo
        int maxy = H_LIFES_TIME + H_PAVEMENT + H_RIVER + H_GRASS + H_DENS + H_SCORE;
        int maxx = (N_DENS * L_FROG) + (4 * L_FROG) + 2;

        // Inizializzazione variabili per la difficoltà
        int choice, udelay, time_limit, time_bonus;

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
            nodelay(stdscr, 1);
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
                    udelay = 300000; time_limit = 99; time_bonus = 50;
                    break;
                case 'm': // Imposta la difficoltà a media
                    udelay = 200000; time_limit = 70; time_bonus = 40;
                    break;
                case 'h': // Imposta la difficoltà a difficile
                    udelay = 100000; time_limit = 50; time_bonus = 30;
                    break;
            }
           
            // Distrugge la finestra di difficoltà dopo la selezione
            delwin(difficulty);
            clear(); // Pulisce lo schermo dopo la chiusura della finestra

            // Crea le pipes
            if (pipe(pipe_to_parent) == -1 || pipe(pipe_to_frog) == -1 || pipe(pipe_to_crocodile) == -1 || pipe(pipe_to_grenade[0]) == -1 || pipe(pipe_to_grenade[1]) == -1 || pipe(pipe_to_bullet) == -1) {
                perror("pipe call");
                exit(1);
            }

            // Imposta pipes non bloccanti
            fcntl(pipe_to_parent[1], F_SETFL, O_NONBLOCK);
            fcntl(pipe_to_frog[0], F_SETFL, O_NONBLOCK);
            fcntl(pipe_to_crocodile[0], F_SETFL, O_NONBLOCK);
            fcntl(pipe_to_grenade[0][0], F_SETFL, O_NONBLOCK);
            fcntl(pipe_to_grenade[1][0], F_SETFL, O_NONBLOCK);
            fcntl(pipe_to_bullet[0], F_SETFL, O_NONBLOCK);

            // Avvia i processi
            // Processo client per la tastiera per il movimento della rana
            pid_client = fork(); 
            if (pid_client < 0) {
                perror("fork call");
                _exit(2);
            } else if (pid_client == 0) {
                execl("/usr/bin/xterm", "xterm", "-e", "./client.out", (char *)NULL); // Avvia automaticamente la schermata
            }

            pid_frog = fork();
            if (pid_frog < 0) {
                perror("fork call");
                _exit(2);
            } else if (pid_frog == 0) {
                frog(pipe_to_parent, pipe_to_frog, pipe_to_grenade); // Processo della rana
            } else {
                for (int i = 0; i < N_CROCODILE; i++) {
                    pid_crocodile[i] = fork();
                    if (pid_crocodile[i] < 0) {
                        perror("fork call");
                        _exit(2);
                    } else if (pid_crocodile[i] == 0) {
                        crocodile(i, pipe_to_parent, pipe_to_crocodile, pipe_to_bullet, udelay); // Processi coccodrilli
                    }
                }
                score = control(pipe_to_parent, pipe_to_frog, pipe_to_crocodile, pipe_to_grenade, pipe_to_bullet, time_limit, time_bonus); // Funzione controllo

                // Termina i processi
                kill(pid_frog, SIGTERM);
                waitpid(pid_frog, NULL, 0);
                for (int i = 0; i < N_CROCODILE; i++) {
                    kill(pid_crocodile[i], SIGTERM);
                    waitpid(pid_crocodile[i], NULL, 0);
                }
                kill(pid_client, SIGTERM);
                waitpid(pid_client, NULL, 0);
            }
        }

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
   
        endwin(); // Chiude ncurses
    }

    exit(0);
}
