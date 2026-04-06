#include <stdio.h>
#include <stdlib.h>

#include "includes.h"

const int INITIAL_MUTEXES = 3;

static Mutex *mutexes;

void init_mutexes() {
    mutexes = malloc(sizeof(Mutex) * INITIAL_MUTEXES); // Initialize an array of initial mutexes

    mutexes[0].name = "userInput";
    mutexes[1].name = "userOutput";
    mutexes[2].name = "file";

    for (int i = 0; i < INITIAL_MUTEXES; i++) {
        mutexes[i].value = 0;
        mutexes[i].owner_pid = -1; // -1 indicates the mutex is unlocked
        mutexes[i].waiting_queue = create_queue(); // Initialize the waiting queue for each mutex
    }
}

void sem_wait_mutex(int pid, char *mutex_name) {
    if (mutex_name == NULL) return;

    for (int i = 0; i < INITIAL_MUTEXES; i++) {
        if (strcmp(mutexes[i].name, mutex_name) == 0) {
            if (mutexes[i].owner_pid == -1) { // Mutex is unlocked
                mutexes[i].owner_pid = pid; // Lock the mutex for the current process
            } else { // Mutex is already locked
                enqueue(mutexes[i].waiting_queue, pid); // Add the process to the waiting queue
            }

            return;
        }
    }
}

void sem_signal_mutex(int pid, char *mutex_name) {
    if (mutex_name == NULL) return;

    for (int i = 0; i < INITIAL_MUTEXES; i++) {
        if (strcmp(mutexes[i].name, mutex_name) == 0) {
            if (mutexes[i].owner_pid == pid) { // Only the owner can release the mutex
                if (!is_empty(mutexes[i].waiting_queue)) { // If there are waiting processes
                    int next_process_id = dequeue(mutexes[i].waiting_queue); // Get the next process in line
                    mutexes[i].owner_pid = next_process_id; // Transfer ownership to the next process
                    
                    // TODO: should also make the process ready instead of waiting
                } else {
                    mutexes[i].owner_pid = -1; // Unlock the mutex if no processes are waiting
                }
                return;
            }
        }
    }
}