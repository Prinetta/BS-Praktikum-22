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

typedef struct command{
    char command[4];
    char key[10];
    char value[10];
} command;

// splitCommand takes a commandString and returns the first three words of the commandString (seperated by space)
// returns three words because in its used context three key words will be needed
int splitCommand(char commandString[], command * current) {
    int indexOfCommandString = 0;
    int indexOfCommand = 0;
    char * pointer;
    for(int i = 0; i < 3; i++) {
        int finishedWord = 0;
        if(i == 0){
            pointer = current->command;
        }
        else if(i == 1){
            pointer = current->key;
        }
        else if(i == 2){
            pointer = current->value;
        }
        while (finishedWord == 0) {
            if (commandString[indexOfCommandString] == ' ' || commandString[indexOfCommandString] == '\n'
            || ((i == 1 &&
                (strstr(current->command, "GET") || strstr(current->command, "DEL"))
                && (commandString[indexOfCommandString+1] == ' ' || commandString[indexOfCommandString+1] == '\n')))) {
                pointer[indexOfCommand] = '\0';
                indexOfCommandString++;
                indexOfCommand = 0;
                finishedWord = 1;
                if(i == 1 &&
                (strstr(current->command, "GET") || strstr(current->command, "DEL"))){
                    printf("loop\n");
                    *current->value = '\0';
                    break;
                }
            } else {
                pointer[indexOfCommand] = commandString[indexOfCommandString];
                indexOfCommandString++;
                indexOfCommand++;
            }
        }
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

        // Lesen von Daten, die der Client schickt
        bytesRead = read(connectionFileDesc, input, BUFFERSIZE);

        initStorage();


        // Zurückschicken der Daten, solange der Client welche schickt (und kein Fehler passiert)
        if(fork() == 0) {
            while (bytesRead > 0) {
                //command * commandStruct = (command * ) malloc(sizeof(command));
                command *commandStruct;
                commandStruct = (command *) malloc(sizeof(command));

                strcpy(commandStruct->command, " ");
                strcpy(commandStruct->key, " ");
                strcpy(commandStruct->value, " ");

                splitCommand((char **) input, commandStruct);



                char * output = malloc(sizeof (char[512]));



                // strcmp had inconsistencies, so strstr instead with \0 to ensure it's a seperate word
                if (strstr(commandStruct->command, "PUT")) {
                    if(put(commandStruct->key, commandStruct->value) != -1) {
                        sprintf(output, "PUT : %s : %s\n", commandStruct->key, commandStruct->value);
                        write(connectionFileDesc, output, sizeof(char[512]));
                    }
                    else{
                        perror("put failed");
                    }
                }
                if (strstr(commandStruct->command, "GET")) { // for some reason only works if u type a third word?? gotta fix
                    char temp[256] = "key_nonexistent\n";
                    get(commandStruct->key, temp);
                    sprintf(output, "GET : %s : %s\n", commandStruct->key, temp);
                    write(connectionFileDesc, output, sizeof (char[512]));
                }
                if (strstr(commandStruct->command, "DEL")) {
                    char temp[256] = "key_nonexistent\n";
                    get(commandStruct->key, temp);
                    if (del(commandStruct->key) == 0) {
                        strcpy(temp, "key_deleted\n");
                    }
                    sprintf(output, "DEL : %s : %s\n", commandStruct->key, temp);
                    write(connectionFileDesc, output, sizeof (char[512]));
                }
                if (strstr(commandStruct->command, "QUIT")) {
                    close(connectionFileDesc);
                    close(rendevouzFileDesc);
                    printf("Client closed.\n");
                    return 0;
                }
                bytesRead = read(connectionFileDesc, input, BUFFERSIZE);
            }
        }
        close(connectionFileDesc);
    }
    detachStorage();
}


