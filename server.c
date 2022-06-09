/*******************************************************************************

  Ein TCP-Echo-Server als iterativer Server: Der Server schickt einfach die
  Daten, die der Client schickt, an den Client zurück.

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "keyValStore.h"
#include "splitCommand.h"
#include "semaphore.h"

#define BUFFERSIZE 1024 // Größe des Buffers
#define PORT 5678

int handleInputs(int connectionFileDesc, int rendevouzFileDesc, char input[], int bytesRead) {
    while (bytesRead > 0) {
        printf("loop\n");
        fflush(stdout);
        char** substrings = splitCommand(input);
        char * command = substrings[0];
        char * key = substrings[1];
        char * value = substrings[2];
        char * output = malloc(sizeof(char[256]));

        if (strcmp(command, "BEG") == 0) {
            printf("BEG\n");
            fflush(stdout);
            enterCriticalArea();
            strcpy(output, "Alleiniger Zugriff aktiviert.\n");
            write(connectionFileDesc, output, strlen(output));
        }
        if (strcmp(command, "END") == 0) {
            printf("END\n");
            fflush(stdout);
            leaveCriticalArea();
            strcpy(output, "Zugriff freigegeben.\n");
            write(connectionFileDesc, output, strlen(output));
        }
        if (strcmp(command, "PUT") == 0) {
            if(put(key, value) == 0) {
                sprintf(output, "PUT : %s : %s\n", key, value);
                write(connectionFileDesc, output, strlen(output));
            }
        }
        if (strcmp(command, "GET") == 0) {
            char temp[64];
            if (get(key, temp) == -1) {
                strcpy(temp, "key_nonexistent");
            }
            sprintf(output, "GET : %s : %s\n", key, temp);
            write(connectionFileDesc, output, strlen(output));
        }
        if (strcmp(command, "DEL") == 0) {
            char temp[64];
            get(key, temp);
            if (del(key) == 0) {
                strcpy(temp, "key_deleted");
            }
            sprintf(output, "DEL : %s : %s\n", key, temp);
            write(connectionFileDesc, output, strlen(output));
        }
        if (strcmp(command, "QUIT") == 0) {
            close(connectionFileDesc);
            close(rendevouzFileDesc);
            printf("Client closed.\n");
            fflush(stdout);
            return 0;
        }
        bytesRead = read(connectionFileDesc, input, BUFFERSIZE);
    }
    return 0;
}

int startServer() {
    int rendevouzFileDesc; // (server socket)
    int connectionFileDesc; // (client socket)

    struct sockaddr_in client; // Client-Socketadresse
    socklen_t clientLength; // Länge der Client-Daten
    char input[BUFFERSIZE]; // Daten vom Client an den Server
    int bytesRead; // Anzahl der Bytes, die der Client geschickt hat


    // Socket erstellen (AF_INET = address family: internet, domain and type describe the protocol,
    // 0 causes default protocol for domain and type to be selected)
    rendevouzFileDesc = socket(AF_INET, SOCK_STREAM, 0);
    if (rendevouzFileDesc < 0){
        fprintf(stderr, "socket konnte nicht erstellt werden\n");
        exit(-1);
    }


    // Socket Optionen setzen für schnelles wiederholtes Binden der Adresse
    int option = 1;
    setsockopt(rendevouzFileDesc, SOL_SOCKET, SO_REUSEADDR, (const void *) &option, sizeof(int));


    // Socket binden
    struct sockaddr_in server;
    server.sin_family = AF_INET; // type of address family that the socket can communicate with (in this case, Internet Protocol v4 addresses)
    server.sin_addr.s_addr = INADDR_ANY; // ip address that is used when we don't want to bind a socket to any specific IP
    server.sin_port = htons(PORT); // htons function can be used to convert an IP port number in host byte order to the IP port number in network byte order
    int couldBindServerAddress = bind(rendevouzFileDesc, (struct sockaddr *) &server, sizeof(server));
    if (couldBindServerAddress < 0){
        fprintf(stderr, "socket konnte nicht gebunden werden\n");
        exit(-1);
    }

    // Server hört zu
    int couldSetServerToListen = listen(rendevouzFileDesc, 5);
    if (couldSetServerToListen < 0){
        fprintf(stderr, "socket konnte nicht listen gesetzt werden\n");
        exit(-1);
    }

    createSemaphore();
    initDataStorage();

    while (1) { // works with space at end of command
        // Verbindung eines Clients wird entgegengenommen
        // (awaits connection to rendevouzFileDesc, opens new socket to communicate with it and saves client address)
        connectionFileDesc = accept(rendevouzFileDesc, (struct sockaddr *) &client, &clientLength);

        // Lesen von Daten, die der Client schickt
        bytesRead = read(connectionFileDesc, input, BUFFERSIZE);

        // Zurückschicken der Daten, solange der Client welche schickt (und kein Fehler passiert)
        if(fork() == 0) {
            initSemaphore();
            return handleInputs(connectionFileDesc, rendevouzFileDesc, input, bytesRead);
        }
        close(connectionFileDesc);
    }
    detachSemaphore();
    detachStorage();
}


