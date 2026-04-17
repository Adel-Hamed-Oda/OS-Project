#ifndef PROCESS_CONTROLLER_H
#define PROCESS_CONTROLLER_H

void initialize_process_controller();
void create_new_process(char *file_name);

extern char ***instructions;
extern int instruction_counts[FILE_COUNT];

#endif