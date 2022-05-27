#include "main.h"
#include <stdio.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "keyValStore.h"
#include "sub.h"

int test() {
    int i, id, *shar_mem;

    int pid[4];

    id = shmget(IPC_PRIVATE, sizeof (int), IPC_CREAT|0600);
    shar_mem = (int *) shmat(id, 0, 0);
    *shar_mem = 0;

    for (i = 0; i < 4; i++) {
        pid[i] = fork();
        if (pid[i] == -1) {
            printf("child failure");
            exit(1);
        }
        if (pid[i] == 0) {
            int count = 0;
            while (*shar_mem < 1000000) {
                *shar_mem += 1;
                count++;
            }
            printf("%i Durchläufe von Kind %d in Prozess %d\n", count, i, getpid());
            exit(0);
        }
    }

    for (i = 0; i < 4; i++) {
        waitpid(pid[i], NULL, 0);
    }

    shmdt(shar_mem);
    shmctl(id, IPC_RMID, 0);
    return 0;
}

int main() {
    //test();
    startServer();
    return 0;
}