#ifndef PROCESS_CONTROLLER_H
#define PROCESS_CONTROLLER_H

typedef struct PCB {
    int pid;
    char *file_name;
    char *state;  // "blocked", "ready", "running", "terminated"
    int program_counter;
    int *memory_boundaries; // [1, 4] means I am occupying memory words 1, 2, and 3
} PCB;

PCB *create_new_PCB(char *file_name);

#endif