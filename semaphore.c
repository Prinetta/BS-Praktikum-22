//
// Created by Daze on 09.06.2022.
//

#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>

int sem_id;
unsigned short marker[1];

struct sembuf enter, leave; // Structs für den Semaphor

void createSemaphore() {
    // Es folgt das Anlegen der Semaphorgruppe. Es wird hier nur ein
    // Semaphor erzeugt
    sem_id = semget(IPC_PRIVATE, 1, IPC_CREAT|0644);
    if (sem_id == -1) {
        perror("Die Gruppe konnte nicht angelegt werden!");
        exit(1);
    }

    // Anschließend wird der Semaphor auf 1 gesetzt
    marker[0] = 1;
    semctl(sem_id, 1, SETALL, marker);  // alle Semaphore auf 1
}

void initSemaphore() {
    enter.sem_num = leave.sem_num = 0;  // Semaphor 0 in der Gruppe
    enter.sem_flg = leave.sem_flg = SEM_UNDO;
    enter.sem_op = -1; // blockieren, DOWN-Operation
    leave.sem_op = 1;   // freigeben, UP-Operation
}

void enterCriticalArea() {
    printf("entered\n");
    fflush(stdout);
    semop(sem_id, &enter, 1); // Eintritt in kritischen Bereich
}

void leaveCriticalArea() {
    printf("left\n");
    fflush(stdout);
    semop(sem_id, &leave, 1);
}

void detachSemaphore() {
    semctl(sem_id, 0, IPC_RMID);
}