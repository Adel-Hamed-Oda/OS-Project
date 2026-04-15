#ifndef PROCESS_CONTROLLER_H
#define PROCESS_CONTROLLER_H

void initialize_process_controller(int max_processes);
char *get_instructions_by_id(int pid);

extern char **instructions;

#endif