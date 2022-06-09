//
// Created by wall on 09.06.2022.
//

#ifndef BS_PRAKTIKUM_22_SUB_H
#define BS_PRAKTIKUM_22_SUB_H

#endif //BS_PRAKTIKUM_22_SUB_H

typedef struct Message{
    long type;
    char text[256];
} Message;

void checkSub(int pid, int connectionFileDesc);
int sub(int pid, char * key, char * value);
void printSubArray();
int initSubStorage();
int detachSubStorage();
int checkNotify(char * key, char * string);
