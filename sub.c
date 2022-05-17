/*******************************************************************************

  Ein TCP-Echo-Server als iterativer Server: Der Server schickt einfach die
  Daten, die der Client schickt, an den Client zurück.

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "keyValStore.h"

#define BUFFERSIZE 1024 // Größe des Buffers
#define PORT 5678


// splitCommand takes a string and returns the first three words of the string (seperated by space)
// returns three words because in its used context three key words will be needed
int splitCommand(char string[], char * substrings[][256]) {
    int index = 0;
    char substring[256] = "";
    for (int i = 0; i < strlen(string) && index < 3; i++) {
        if (string[i] == ' ') {
            strcpy((char *) substrings[index], (const char *) &substring);
            strcpy(substring, "");
            index++;
        } else {
            strncat(substring, &string[i], 1);
        }
    }
    if (index < 3 && strlen(substring) > 0) {
        strcpy((char *) substrings[index], (const char *) &substring);
    }
    return 0;
}

int startServer() {

    int rendevouzFileDesc; // Rendevouz-Descriptor (server socket)
    int connectionFileDesc; // Verbindungs-Descriptor (client socket)

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
    if (couldSetServerToListen < 0 ){
        fprintf(stderr, "socket konnte nicht listen gesetzt werden\n");
        exit(-1);
    }

    while (1) {
        // Verbindung eines Clients wird entgegengenommen
        // (awaits connection to rendevouzFileDesc, opens new socket to communicate with it and saves client address)
        connectionFileDesc = accept(rendevouzFileDesc, (struct sockaddr *) &client, &clientLength);
        printf("Die Client ist connected\n");

        // Lesen von Daten, die der Client schickt
        bytesRead = read(connectionFileDesc, input, BUFFERSIZE);

        // Zurückschicken der Daten, solange der Client welche schickt (und kein Fehler passiert)
        if(fork() == 0) {
            while (bytesRead > 0) {
                char *substrings[3][256] = {NULL};
                splitCommand(input, substrings);
                char command[256] = "command";
                strcpy(command, (const char *) substrings[0]);
                char key[256] = "key";
                strcpy(key, (const char *) substrings[1]);
                char value[256] = "value";
                strcpy(value, (const char *) substrings[2]);
                // strcmp had inconsistencies, so strstr instead with \0 to ensure it's a seperate word
                if (strstr(command, "PUT")) {
                    put(key, value);
                } if (strstr(command, "GET")) { // for some reason only works if u type a third word?? gotta fix
                    char temp[256] = "hi";
                    get(key, temp);
                    printf("hii %s\n", temp);
                }
                if (strstr(command, "DEL")) {
                    del(key);
                }
                if (strstr(command, "QUIT")) {

                    close(connectionFileDesc);
                    close(rendevouzFileDesc);
                    printf("Close");
                    return 0;
                } // telnet localhost 5678

                //printf("sending back the %d bytes I received...\n", bytesRead);
                write(connectionFileDesc, input, bytesRead);
                bytesRead = read(connectionFileDesc, input, BUFFERSIZE);
            }
            close(connectionFileDesc);
        }

    }



}


