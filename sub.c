//
// Created by wall on 09.06.2022.
//

#include <stdio.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/msg.h>
#include "keyValStore.h"
#include "server.h"

#define DATA_ARRAY_SIZE 100
#define DATA_ARRAY_BYTES sizeof(Sub [DATA_ARRAY_SIZE])
#define DEFAULT_KEY "no_key"
#define DEFAULT_VALUE -1
#define DEFAULT_DATA (Sub) { .key = DEFAULT_KEY, .pid = DEFAULT_VALUE }

typedef struct Sub {
    char key[10];
    int pid;
} Sub;

typedef struct Message{
    int type;
    char text[100];
} Message;

Sub * subsArray;
int storageId;

void printSubArray() {
    for (int i = 0; i < 100; ++i) {
        if(strcmp(subsArray[i].key, "no_key") != 0) {
            printf("%s\n", subsArray[i].key);
            printf("%d\n", subsArray[i].pid);
            fflush(stdout);
        }
    }
    printf("----------------\n");
}

int initSubStorage() {
    storageId = shmget(IPC_PRIVATE, DATA_ARRAY_BYTES, IPC_CREAT|0600);
    if(storageId == -1){
        perror("shmget failed");
        return -1;
    }

    subsArray = shmat(storageId, 0, 0);
    if(subsArray == (void * ) -1){
        perror("shmat failed");
        return -1;
    }
    for (int i = 0; i < DATA_ARRAY_SIZE; ++i) {
        subsArray[i] = DEFAULT_DATA;
    }

    return 0;
}

int detachSubStorage() {
    if (shmdt(subsArray) == -1) {
        perror("shmdt failed");
        return -1;
    }
    if (shmctl(storageId, IPC_RMID, 0) == -1) {
        perror("shmdt failed");
        return -1;
    }
    return 0;
}

int nextEmptySubIndex() {
    for (int i = 0; i < DATA_ARRAY_SIZE; ++i) {
        if (strcmp(subsArray[i].key, DEFAULT_KEY) == 0) {
            return i;
        }
    }
    return -1;
}

int sub(int pid, char * key, char * value){
    char temp[64];
    if(get(key, temp) != 0){
        return -1;
    }

    strcpy(value, temp);


    int index = nextEmptySubIndex();
    if (index != -1) {
        subsArray[index] = (Sub) {};
        strcpy(subsArray[index].key, key);
        subsArray[index].pid = pid;
        printSubArray();
        return 0;
    }

    return -1;
}

int notify(int pid){
    int msid = msgget(MSG_KEY, 0);
    if(msid == -1){
        printf("cannot get message queue\n");
    }
    Message message = (Message){pid, "TEST TEST TEST"};
    printf("Message Type: %d\n", message.type);
    int send = msgsnd(msid, &message, sizeof(message), 0);
    if(send < 0){
        printf("error writing to queue\n");
        printf("status: %d\n", send);
    }
    printf("message sent to: %d", pid);
}

int checkNotify(char * key){
    for(int i = 0; i < DATA_ARRAY_SIZE; ++i){
        if(strcmp(subsArray[i].key, key) == 0){
            notify(subsArray[i].pid);
        }
    }
    //bei delete müssen alle dingens mit key gelöscht werden
}