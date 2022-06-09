//
// Created by wall on 09.06.2022.
//

#ifndef BS_PRAKTIKUM_22_SUB_H
#define BS_PRAKTIKUM_22_SUB_H

#endif //BS_PRAKTIKUM_22_SUB_H

typedef struct Message{
    int type;
    char text[100];
} Message;

int sub();
int initSubStorage();
int detachSubStorage();
int checkNotify();
