//
// Created by wall on 08.04.2022.
//

#include "keyValStore.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>

typedef struct data{
    char * key[10];
    char * value[10];
} data;

data * dataArray[100] = {NULL};


int put(char * key, char * value){
    char * res = NULL;
    if(get(key, res) == 0 ){
        return -1;
    }

    int next = 0;
    for (int i = 0; i < 100; ++i) {
        if( dataArray[i] == NULL){
            next = i;
            break;
        }
    }
    printf("%d", next);
    data new = {.key = key, .value = value};
    printf("\n%s, %s", new.key, new.value);
    * dataArray[next] = new;

    return  0;
};
int get(char * key, char * res){
    for (int i =0; i < 100; i++) {
        if(dataArray[i] != NULL && strcmp ((const char *) dataArray[i]->key, key) == 0){
            strcpy(res, (char *) dataArray[i]->value );
            return 0;
        }
    }
    return -1;
};
int del(char * key);
