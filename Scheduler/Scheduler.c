#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Includes.h"

int current_process_id = -1;
int time = 0;
const int TIME_QUANTUM = 2; // You can change the Round Robin time quantum here

Queue *ready_queue;
Queue *input_blocked_queue;
Queue *output_blocked_queue;
Queue *file_blocked_queue;

// Process states for readability
const char* STATE_READY      = "1";
const char* STATE_RUNNING    = "2";
const char* STATE_BLOCKED    = "3";
const char* STATE_TERMINATED = "4";

void advance_time() {    
    time++;

    scanf("%*c"); // Wait for user input to advance time (for step-by-step execution)
}

void initialize_scheduler() {
    ready_queue = create_queue();
    input_blocked_queue = create_queue();
    output_blocked_queue = create_queue();
    file_blocked_queue = create_queue();
}

void block_process(int pid, char *mutex_name) {
    if (strcmp(mutex_name, "userInput") == 0) {
        enqueue(input_blocked_queue, pid);
    } else if (strcmp(mutex_name, "userOutput") == 0) {
        enqueue(output_blocked_queue, pid);
    } else if (strcmp(mutex_name, "file") == 0) {
        enqueue(file_blocked_queue, pid);
    }
    
    // Change state in memory to Blocked if it's currently loaded
    int start, end;
    get_process_bounds(pid, &start, &end);
    if (start != -1) {
        if (memory[start + 1].value != NULL) free(memory[start + 1].value);
        memory[start + 1].value = strdup(STATE_BLOCKED);
    }

    // Force context switch
    current_process_id = -1;
}

void run_round_robin_scheduler() {
    int completed_processes = 0;
    int current_quantum_used = 0;

    printf("--- Starting Round Robin Scheduler (Quantum: %d) ---\n", TIME_QUANTUM);

    while (completed_processes < FILE_COUNT) {
        
        // 1. Check for New Process Arrivals at current time unit
        for (int i = 0; i < FILE_COUNT; i++) {
            if (LOAD_TIMES[i] == time) {
                printf("[Time %d] Process %d arrived.\n", time, i);
                
                // Load into memory (try_looking_for_context = 0 since it's brand new)
                if (try_load_process_into_memory(i, 0) != -1) {
                    change_process_state(i, STATE_READY);
                } else {
                    printf("[Time %d] Failed to load Process %d (Memory Full).\n", time, i);
                }
                
                enqueue(ready_queue, i);
            }
        }

        // 2. Check if current process exhausted its time quantum
        if (current_process_id != -1 && current_quantum_used == TIME_QUANTUM) {
            printf("[Time %d] Process %d exhausted quantum. Context switching...\n", time, current_process_id);
            change_process_state(current_process_id, STATE_READY);
            enqueue(ready_queue, current_process_id);
            
            current_process_id = -1; // Deschedule
        }

        // 3. Schedule the next process from the Ready Queue if CPU is idle
        if (current_process_id == -1 && !is_empty(ready_queue)) {
            current_process_id = dequeue(ready_queue);
            current_quantum_used = 0;

            // Ensure the scheduled process is in memory before running.
            // If it was swapped out, this function will pull its context back from disk.
            int start, end;
            get_process_bounds(current_process_id, &start, &end);
            
            if (start == -1) {
                printf("[Time %d] Process %d is not in memory. Swapping IN...\n", time, current_process_id);
                try_load_process_into_memory(current_process_id, 1);
            }
            
            change_process_state(current_process_id, STATE_RUNNING);
            printf("[Time %d] Scheduled Process %d.\n", time, current_process_id);
        }

        // 4. Execute an instruction for the current running process
        if (current_process_id != -1) {
            int start, end;
            get_process_bounds(current_process_id, &start, &end);
            
            if (start != -1) {
                // Fetch the Program Counter (PC)
                int pc = atoi((char *)memory[start + 3].value);
                
                // Instructions start at index (start + 4 for PCB + 3 for Variables) -> start + 7
                char *instruction = memory[start + 7 + pc].value;

                printf("[Time %d] Process %d executing [PC: %d]: %s\n", time, current_process_id, pc, instruction);
                
                // Parse and execute the instruction
                parse_line(instruction);

                // Increment PC and Quantum tracking
                pc++;
                current_quantum_used++;

                // Update the PC in memory
                char pc_str[16];
                snprintf(pc_str, 16, "%d", pc);
                if (memory[start + 3].value != NULL) free(memory[start + 3].value);
                memory[start + 3].value = strdup(pc_str);

                // Check if the process has finished all its instructions
                if (pc >= instruction_counts[current_process_id]) {
                    printf("[Time %d] Process %d Finished Execution.\n", time, current_process_id);
                    change_process_state(current_process_id, STATE_TERMINATED);
                    
                    // Clear it from memory to free up space
                    for(int i = start; i <= end; i++) {
                        if (memory[i].name) free(memory[i].name);
                        if (memory[i].value) free(memory[i].value);
                        memory[i].name = NULL;
                        memory[i].value = NULL;
                        memory[i].type = 0;
                    }
                    compact_memory();
                    
                    current_process_id = -1;
                    completed_processes++;
                }
            } else {
                // If it mysteriously disappeared from bounds, force deschedule
                current_process_id = -1;
            }
        } else {
            // CPU is idling
            printf("[Time %d] CPU Idle.\n", time);
        }

        // 5. Advance time unit
        advance_time();
    }

    printf("--- Scheduling Complete at Time %d ---\n", time);
}