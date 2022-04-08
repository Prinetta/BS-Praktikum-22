//
// Created by wall on 08.04.2022.
//

#include "keyValStore.h"

typedef struct data{
    char * key[10];
    char * value[10];
} data;

data * dataArray[100];


int put(char * key, char * value);
int get(char * key, char * res);
int del(char * key);
