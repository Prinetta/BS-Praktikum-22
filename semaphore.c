//
// Created by Daze on 09.06.2022.
//

#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>

int pid_id, * currentPid;
int sem_id;
unsigned short marker[1];

struct sembuf enter; // Structs für den Semaphor

void createSemaphore() {
    pid_id = shmget(IPC_PRIVATE, sizeof (int), IPC_CREAT | 0600);
    if(pid_id == -1){
        perror("shmget failed");
        exit(1);
    }

    currentPid = (int *) shmat(pid_id, 0, 0);
    if(currentPid == (void * ) -1){
        perror("shmat failed");
        exit(1);
    }

    * currentPid = -1;

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
    enter.sem_num = 0;  // Semaphor 0 in der Gruppe
    enter.sem_flg = SEM_UNDO;
    enter.sem_op = 1;
}

void enterCriticalArea() {
    * currentPid = getpid();
    semop(sem_id, &enter, 1); // Eintritt in kritischen Bereich
}

void leaveCriticalArea() {
    * currentPid = -1;
}

int hasAccess() {
    return * currentPid == getpid() || * currentPid == -1;
}

void detachSemaphore() {
    semctl(sem_id, 0, IPC_RMID);
}