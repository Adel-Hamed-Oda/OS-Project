#include <stdio.h>
#include <stdlib.h>

#include "Process Controller.h"

static int next_pid = 0; // this will be used to assign unique PIDs to each process

struct PCB {
    int pid;
    char *file_name;
    char *state;  // "ready", "running", "terminated", IDK wether I should add waiting
    int program_counter;
    int *memory_boundaries; // [1, 2, 3] means I am occupying memory words 1, 2, and 3
};

struct PCB *create_new_PCB(char *file_name) {
    struct PCB *new_pcb = malloc(sizeof(struct PCB));
    new_pcb->pid = next_pid++;
    new_pcb->file_name = file_name;
    new_pcb->state = "ready";
    new_pcb->program_counter = 0;
    new_pcb->memory_boundaries = NULL;
    return new_pcb;
}