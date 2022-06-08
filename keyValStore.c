//
// Created by wall on 08.04.2022.
//

#include "keyValStore.h"
#include <stddef.h>
#include <string.h>
#include <malloc.h>
#include <sys/shm.h>

typedef struct data{
    char key[10];
    char value[10];
} data;

data * dataArray[100];

int storageId;

int initStorage() {
    * dataArray = (data *) malloc(100 * sizeof(data));
    storageId = shmget(IPC_PRIVATE, sizeof (dataArray), IPC_CREAT|0600);
    if(storageId == -1){
        perror("shmget failed");
        return -1;
    }

    * dataArray = shmat(storageId, 0, 0);
    if(dataArray == (void * ) -1){
        perror("shmat failed");
        return -1;
    }
    return 0;
}

int detachStorage() {
    if (shmdt(dataArray) == -1) {
        perror("shmdt failed");
        return -1;
    }
    if (shmctl(storageId, IPC_RMID, 0) == -1) {
        perror("shmdt failed");
        return -1;
    }
    return 0;
}

int indexOf(char * key) {
    for (int i = 0; i < 100; i++) {
        if (dataArray[i] != NULL && strcmp (dataArray[i]->key, key) == 0) {
            return i;
        }
    }
    return -1;
}

int nextEmptyIndex() {
    for (int i = 0; i < 100; i++) {
        if (dataArray[i] == NULL) {
            return i;
        }
    }
    return -1;
}

int put(char * key, char * value) {
    char * res = NULL;
    if(get(key, res) == 0){
        strcpy(res, value);
        return 0;
    }

    int next = nextEmptyIndex();
    if (next == -1) {
        return -1;
    } else {
        int size = sizeof(key);
        data * new = (data *) malloc(sizeof (data));
        strcpy(new->key, key);
        strcpy(new->value, value);
        new->key[size-1] = '\0';
        new->value[size-1] = '\0';
        dataArray[next] = new;
        //free(new);
    }

    return 0;
}

int get(char * key, char * res){
    int index = indexOf(key);
    if (index >= 0) {
        strcpy(res, dataArray[index]->value);
        return 0;
    }
    return -1;
}

int del(char * key) {
    int index = indexOf(key);
    if (index >= 0) {
        dataArray[index] = NULL;
        return 0;
    }
    return -1;
}
