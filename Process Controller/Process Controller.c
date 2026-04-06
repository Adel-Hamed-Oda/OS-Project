#include <stdio.h>
#include <stdlib.h>

#include "includes.h"

static int next_pid = 0; // this will be used to assign unique PIDs to each process
static PCB *pcbs; // this is a simple process table to keep track of all processes, indexed by PID

void initialize_process_table(int max_processes) {
    pcbs = malloc(sizeof(PCB) * max_processes);
}

void create_new_PCB(char *file_name) {
    PCB *new_pcb = malloc(sizeof(PCB));
    new_pcb->pid = next_pid++;
    new_pcb->file_name = file_name;
    new_pcb->state = "ready";
    new_pcb->program_counter = 0;
    new_pcb->memory_boundaries = NULL;
    
    pcbs[new_pcb->pid] = *new_pcb;
}

PCB *get_PCB_by_pid(int pid) {
    return &pcbs[pid];
}