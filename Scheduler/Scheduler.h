#ifndef SCHEDULER_H
#define SCHEDULER_H

extern int current_process_id;

void initialize_scheduler();

void block_process(int pid, char *mutex_name);

void run_round_robin_scheduler();

#endif