#include <stdio.h>
#include <stdlib.h>

#include "Includes.h"

int current_process_id = -1;

const int MAX_TIME = 100;
const char *SCHEDULING_ALGORITHM = "FCFS"; // "FCFS", "RR", "HRRN", or "MLFQ"

int time = 0;
Queue *ready_queue;
Queue *input_blocked_queue;
Queue *output_blocked_queue;
Queue *file_blocked_queue;
Queue *job_queue;

void advance_time() {    
    time++;
}

void initialize_scheduler() {
    ready_queue = create_queue();
    input_blocked_queue = create_queue();
    output_blocked_queue = create_queue();
    file_blocked_queue = create_queue();
    job_queue = create_queue();
}

void block_process(int pid, char *mutex_name) {
    if (strcmp(mutex_name, "userInput") == 0) {
        enqueue(input_blocked_queue, pid);
    } else if (strcmp(mutex_name, "userOutput") == 0) {
        enqueue(output_blocked_queue, pid);
    } else if (strcmp(mutex_name, "file") == 0) {
        enqueue(file_blocked_queue, pid);
    }
}