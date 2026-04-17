#ifndef MUTEX_MANAGER_H
#define MUTEX_MANAGER_H

typedef struct Mutex {
    char *name;
    int owner_pid; // PID of the process that currently owns the mutex, -1 if
} Mutex;

// Function declarations for mutex management
void init_mutexes();
int sem_wait_mutex(int pid, char *mutex_name);
int sem_signal_mutex(int pid, char *mutex_name);

#endif // MUTEX_MANAGER_H