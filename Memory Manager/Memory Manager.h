#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

void initialize_memory();

int try_load_process_into_memory(int pid, int try_looking_for_context);

int allocate_process(int pid, int start);

void get_process_bounds(int pid, int *start, int *end);

void save_context(int pid);

void load_context(int pid, int start);

#endif