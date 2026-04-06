#ifndef MUTEX_MANAGER_H
#define MUTEX_MANAGER_H

typedef struct Mutex {
    char *name;
    int value;
    int owner_pid; // PID of the process that currently owns the mutex, -1 if 
    Queue *waiting_queue; // Queue of processes waiting for this mutex
} Mutex;

// Function declarations for mutex management
void init_mutexes();
void sem_wait_mutex(int pid, char *mutex_name);
void release_mutex(int pid, char *mutex_name);

#endif // MUTEX_MANAGER_H