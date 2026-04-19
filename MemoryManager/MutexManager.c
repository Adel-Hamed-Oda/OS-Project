#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Includes.h"

// Replaced const int with #define to prevent compiler/linker confusion
#define INITIAL_MUTEXES 3

static Mutex *mutexes;

void initialize_mutexes() {
    mutexes = malloc(sizeof(Mutex) * INITIAL_MUTEXES);

    mutexes[0].name = "userInput";
    mutexes[1].name = "userOutput";
    mutexes[2].name = "file";

    for (int i = 0; i < INITIAL_MUTEXES; i++) {
        mutexes[i].owner_pid = -1; // -1 indicates the mutex is unlocked
    }
}

int sem_wait_mutex(int pid, char *mutex_name) {
    if (mutex_name == NULL) return -1;

    for (int i = 0; i < 3; i++) {
        if (strcmp(mutexes[i].name, mutex_name) == 0) {
            if (mutexes[i].owner_pid == -1) {
                mutexes[i].owner_pid = pid; // Lock the mutex
                return 0; // Success
            }
            return -1; // Mutex found, but it is locked (Blocked)
        }
    }

    // If we reach here, the mutex name wasn't found in the array!
    fprintf(stderr, "WARNING: Process %d tried to wait on an unknown mutex: '%s'\n", pid, mutex_name);
    return -1; 
}

int sem_signal_mutex(int pid, char *mutex_name) {
    if (mutex_name == NULL) return -1;

    for (int i = 0; i < INITIAL_MUTEXES; i++) {
        if (strcmp(mutexes[i].name, mutex_name) == 0) {
            if (mutexes[i].owner_pid == pid) { // Only the owner can release it
                
                mutexes[i].owner_pid = -1; // Unlock the mutex
                return 0; // Success
                
            } else {
                fprintf(stderr, "WARNING: Process %d tried to unlock '%s', but it is owned by Process %d!\n", 
                        pid, mutex_name, mutexes[i].owner_pid);
                return -1; // Failed to unlock
            }
        }
    }

    fprintf(stderr, "WARNING: Process %d tried to signal an unknown mutex: '%s'\n", pid, mutex_name);
    return -1; 
}