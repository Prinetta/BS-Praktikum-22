//
// Created by wall on 08.04.2022.
//

#include "keyValStore.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>

typedef struct data{
    char key[10];
    char value[10];
} data;

data * dataArray[100] = {NULL};

int indexOf(char * key) {
    for (int i = 0; i < 100; i++) {
        if (dataArray[i] != NULL && strcmp ((const char *) dataArray[i]->key, key) == 0) {
            return 0;
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
        res = value;
        return 0;
    }

    int next = nextEmptyIndex();
    if (next == -1) {
        return -1;
    } else {
        data new = {};
        strcpy(new.key, key);
        strcpy(new.value, value);
        dataArray[next] = &new;
    }

    return 0;
}

int get(char * key, char * res){
    int index = indexOf(key);
    if (index >= 0) {
        strcpy(res, dataArray[index]->value);
        return 0;
    }
    strcpy(res, "");
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
