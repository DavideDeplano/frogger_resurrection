#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>

int main() {
    struct sockaddr_in serverAddr;
    int clientSocket, c;

    // Creazione socket client
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket < 0) {
        perror("socket() failed");
        exit(1);
    }

    // Configurazione indirizzo del server
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Usa 127.0.0.1 per connessione locale
    serverAddr.sin_port = htons(8088);

    // Tentativo di connessione al server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("connect() failed");
        exit(2);
    }

    // Inizializza ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, 1);
    curs_set(0);
    start_color();

    timeout(150);

    // Variabili per la dimensione dello schermo
    int maxy, maxx;
    getmaxyx(stdscr, maxy, maxx);

    int key_width = maxx / 3;    
    int key_height = maxy / 3;    
    int space_width = key_width * 3;        
    int space_height = key_height; 

    // Coppie di colori per i tasti
    init_pair(1, COLOR_BLACK, COLOR_GREEN);
    init_pair(2, COLOR_BLACK, COLOR_RED);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);

    // Sfondo
    bkgd(COLOR_PAIR(3));
    refresh();

    // Inizializza le finestre per i tasti
    WINDOW *key_up = newwin(key_height, key_width, 0, key_width);
    WINDOW *key_left = newwin(key_height, key_width, key_height, 0);
    WINDOW *key_down = newwin(key_height, key_width, key_height, key_width);
    WINDOW *key_right = newwin(key_height, key_width, key_height, 2 * key_width);
    WINDOW *space_bar = newwin(space_height, space_width, 2 * key_height, 0);

    // Colora le finestre
    wbkgd(key_up, COLOR_PAIR(1));
    wbkgd(key_left, COLOR_PAIR(1));
    wbkgd(key_down, COLOR_PAIR(1));
    wbkgd(key_right, COLOR_PAIR(1));
    wbkgd(space_bar, COLOR_PAIR(1));

    // Stampa all'interno delle finestre
    mvwprintw(key_up, key_height / 2, key_width / 2 - 1, "UP");
    mvwprintw(key_left, key_height / 2, key_width / 2 - 2, "LEFT");
    mvwprintw(key_down, key_height / 2, key_width / 2 - 2, "DOWN");
    mvwprintw(key_right, key_height / 2, key_width / 2 - 2, "RIGHT");
    mvwprintw(space_bar, space_height / 2, space_width / 2 - 5, "SPACE");

    // Bordi 
    box(key_up, ACS_VLINE, ACS_HLINE);
    box(key_left, ACS_VLINE, ACS_HLINE);
    box(key_down, ACS_VLINE, ACS_HLINE);
    box(key_right, ACS_VLINE, ACS_HLINE);
    box(space_bar, ACS_VLINE, ACS_HLINE);

    // Aggiorna
    wrefresh(key_up);
    wrefresh(key_left);
    wrefresh(key_down);
    wrefresh(key_right);
    wrefresh(space_bar);

    while (1) {
        int c = getch(); // Legge il comando da input

        // In base al tasto premuto c assume il valore e la grafica del tasto cambia colore per un attimo
        if (c == KEY_UP) {
            wbkgd(key_up, COLOR_PAIR(2));
            wrefresh(key_up);
            napms(200);
            wbkgd(key_up, COLOR_PAIR(1));
            wrefresh(key_up);
        } else if (c == KEY_DOWN) {
            wbkgd(key_down, COLOR_PAIR(2));
            wrefresh(key_down);
            napms(200);
            wbkgd(key_down, COLOR_PAIR(1));
            wrefresh(key_down);
        } else if (c == KEY_LEFT) {
            wbkgd(key_left, COLOR_PAIR(2));
            wrefresh(key_left);
            napms(200);
            wbkgd(key_left, COLOR_PAIR(1));
            wrefresh(key_left);
        } else if (c == KEY_RIGHT) {
            wbkgd(key_right, COLOR_PAIR(2));
            wrefresh(key_right);
            napms(200);
            wbkgd(key_right, COLOR_PAIR(1));
            wrefresh(key_right);
        } else if (c == ' ') {
            wbkgd(space_bar, COLOR_PAIR(2));
            wrefresh(space_bar);
            napms(200);
            wbkgd(space_bar, COLOR_PAIR(1));
            wrefresh(space_bar);
        } else if (c == 'q') {
            break;
        }

        // Invia il comando al client
        int len = send(clientSocket, &c, sizeof(int), 0);
        if (len != sizeof(int)) {
            perror("send() failed");
            break;
        }
    }

    // Chiude client elimina le finestre e chiude ncurses
    close(clientSocket);
    delwin(key_up);
    delwin(key_left);
    delwin(key_down);
    delwin(key_right);
    delwin(space_bar);
    endwin();
    return 0;
}

