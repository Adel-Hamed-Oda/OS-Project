#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Includes.h"

const int INITIAL_MUTEXES = 3;

static Mutex *mutexes;

void initialize_mutexes() {
    mutexes = malloc(sizeof(Mutex) * INITIAL_MUTEXES); // Initialize an array of initial mutexes

    mutexes[0].name = "userInput";
    mutexes[1].name = "userOutput";
    mutexes[2].name = "file";

    for (int i = 0; i < INITIAL_MUTEXES; i++) {
        mutexes[i].owner_pid = -1; // -1 indicates the mutex is unlocked
    }
}

int sem_wait_mutex(int pid, char *mutex_name) {
    if (mutex_name == NULL) return -1;

    for (int i = 0; i < INITIAL_MUTEXES; i++) {
        if (strcmp(mutexes[i].name, mutex_name) == 0) {
            if (mutexes[i].owner_pid == -1) { // Mutex is unlocked
                mutexes[i].owner_pid = pid; // Lock the mutex for the current process
                return 1; // process has been locked successfully
            }
        }
    }

    return 0; // process has been blocked
}

int sem_signal_mutex(int pid, char *mutex_name) {
    if (mutex_name == NULL) return -1;

    for (int i = 0; i < INITIAL_MUTEXES; i++) {
        if (strcmp(mutexes[i].name, mutex_name) == 0) {
            if (mutexes[i].owner_pid == pid) { // Only the owner can release the mutex
                if (mutexes[i].owner_pid != -1) {
                    mutexes[i].owner_pid = -1; // Unlock the mutex
                }

                return 1; // process has been unlocked successfully
            }
        }
    }

    return 0; // process has not been unlocked
}