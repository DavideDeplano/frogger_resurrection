#include "header.h"

int server(int *c) {
    static int serverSocket = -1, clientSocket = -1; // Variabili statiche per il socket del server e del client
    struct sockaddr_in serverAddr, clientAddr; // Strutture per indirizzi di server e client

    if (serverSocket == -1) {
        // Creazione del socket del server solo se non è già stato creato
        serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocket < 0) {
            perror("socket() failed"); // Log di errore se la creazione del socket fallisce
            exit(1);
        }

        // Impostazione dell'indirizzo del server per ascoltare su qualsiasi interfaccia
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddr.sin_port = htons(8088);

        // Associazione del socket a un indirizzo e una porta
        if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            perror("bind() failed"); // Log di errore se l'associazione fallisce
            exit(2);
        }

        // Configurazione del socket per ascoltare le connessioni in entrata
        if (listen(serverSocket, MAX_PENDING) < 0) {
            perror("listen() failed"); // Log di errore se l'ascolto fallisce
            exit(3);
        }
    }

    if (clientSocket == -1) {
        // Accettazione di un nuovo client solo se non esiste già una connessione
        socklen_t clientLen = sizeof(clientAddr);
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket < 0) {
            exit(4); // Uscita se l'accettazione del client fallisce
        }
    }

    // Ricezione di un intero dal client
    int recvInt = recv(clientSocket, c, sizeof(int), 0);
    if (recvInt > 0) {
        return 1; // Ritorna 1 se un comando è stato ricevuto correttamente
    } else if (recvInt == 0) {
        // Gestione della disconnessione del client
        close(clientSocket);
        clientSocket = -1;
        return 0; // Ritorna 0 se il client si è disconnesso
    } else {
        perror("recv() failed"); // Log di errore se la ricezione fallisce
        return -1; // Ritorna -1 in caso di errore di ricezione
    }

    // Sezione aggiunta per chiudere i socket (questa parte potrebbe non essere raggiunta mai nel codice attuale)
    if (clientSocket != -1) {
        close(clientSocket); // Chiudi il socket del client
    }

    if (serverSocket != -1) {
        close(serverSocket); // Chiudi il socket del server
    }
}
