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

typedef struct dataList {
    data * list[100];
} dataList;

int storageId;
dataList * dataArray;

int initStorage() {
    storageId = shmget(IPC_PRIVATE, sizeof (dataList), IPC_CREAT|0600);
    dataArray = (dataList *) shmat(storageId, 0, 0);
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
        if (dataArray->list[i] != NULL && strcmp (dataArray->list[i]->key, key) == 0) {
            return i;
        }
    }
    return -1;
}

int nextEmptyIndex() {
    for (int i = 0; i < 100; i++) {
        if (dataArray->list[i] == NULL) {
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
        data * new = (data *) malloc(sizeof (data));
        strcpy(new->key, key);
        strcpy(new->value, value);
        dataArray->list[next] = new;
    }

    return 0;
}

int get(char * key, char * res){
    int index = indexOf(key);
    if (index >= 0) {
        strcpy(res, dataArray->list[index]->value);
        return 0;
    }
    return -1;
}

int del(char * key) {
    int index = indexOf(key);
    if (index >= 0) {
        dataArray->list[index] = NULL;
        return 0;
    }
    return -1;
}
