#include <stdio.h>
#include <stdlib.h>

#include "includes.h"

const int MAX_TIME = 100;
const char *SCHEDULING_ALGORITHM = "FCFS"; // "FCFS", "RR", "HRRN", or "MLFQ"

int time = 0;
Queue *ready_queue;
Queue *blocked_queue;

void advance_time() {
    parse_line(NULL);
    
    time++;
}

void schedule_FCFS() {
    
}