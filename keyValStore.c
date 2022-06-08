//
// Created by Daze on 08.06.2022.
//

#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <sys/shm.h>

#define DATA_ARRAY_SIZE 100
#define DATA_ARRAY_BYTES sizeof(Data[DATA_ARRAY_SIZE])
#define DEFAULT_KEY "no_key"
#define DEFAULT_VALUE "no_value"
#define DEFAULT_DATA (Data) { .key = DEFAULT_KEY, .value = DEFAULT_VALUE }

typedef struct Data {
    char key[10];
    char value[10];
} Data;

Data * dataArray;
int storageId;

int initDataStorage() {
    storageId = shmget(IPC_PRIVATE, DATA_ARRAY_BYTES, IPC_CREAT|0600);
    if(storageId == -1){
        perror("shmget failed");
        return -1;
    }

    dataArray = shmat(storageId, 0, 0);
    if(dataArray == (void * ) -1){
        perror("shmat failed");
        return -1;
    }
    for (int i = 0; i < DATA_ARRAY_SIZE; ++i) {
        dataArray[i] = DEFAULT_DATA;
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

void printArray() {
    for (int i = 0; i < 100; ++i) {
        printf("%s\n", dataArray[i].key);
        printf("%s\n", dataArray[i].value);
        fflush(stdout);
    }
}

int indexOf(char * key) {
    for (int i = 0; i < DATA_ARRAY_SIZE; ++i) {
        if (strcmp(dataArray[i].key, key) == 0) {
            return i;
        }
    }
    return -1;
}

int nextEmptyIndex() {
    for (int i = 0; i < DATA_ARRAY_SIZE; ++i) {
        if (strcmp(dataArray[i].key, DEFAULT_KEY) == 0) {
            return i;
        }
    }
    return -1;
}

int put(char * key, char * value) {
    int index = indexOf(key);
    if (index != -1) {
        strcpy(dataArray[index].value, value);
        return 0;
    }

    index = nextEmptyIndex();
    if (index != -1) {
        dataArray[index] = (Data) {};
        strcpy(dataArray[index].key, key);
        strcpy(dataArray[index].value, value);
        return 0;
    }

    return -1;
}

int get(char * key, char * res) {
    for (int i = 0; i < DATA_ARRAY_SIZE; ++i) {
        if (strcmp(dataArray[i].key, key) == 0) {
            strcpy(res, dataArray[i].value);
            return 0;
        }
    }
    return -1;
}

int del(char * key) {
    int index = indexOf(key);
    if (index != -1) {
        dataArray[index] = DEFAULT_DATA;
        return 0;
    }
    return -1;
}