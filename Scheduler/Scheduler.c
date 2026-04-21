#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "Includes.h"

const int RR_TIME_QUANTUM = 2; // You can change the Round Robin time quantum here
const int MLFQ_TIME_QUANTUMS[] = {1, 2, 4, 8}; // Quantum for each MLFQ level

int current_process_id = -1;
int time = 0;

Queue *ready_queue[4];
Queue *input_blocked_queue;
Queue *output_blocked_queue;
Queue *file_blocked_queue;

void advance_time() {
    time++;

    if (TESTING_MODE) {
        fflush(stdout);
        printf("\n[Time %d] Press Enter to continue...", time);
        fflush(stdout);

        getchar();

        Sleep(1000);
    }
}

void initialize_scheduler() {
    for (int i = 0; i < 4; i++) {
        ready_queue[i] = create_queue();
    }
    input_blocked_queue = create_queue();
    output_blocked_queue = create_queue();
    file_blocked_queue = create_queue();
}

void block_process(char *mutex_name) {
    if (strcmp(mutex_name, "userInput") == 0) {
        enqueue(input_blocked_queue, current_process_id);
    } else if (strcmp(mutex_name, "userOutput") == 0) {
        enqueue(output_blocked_queue, current_process_id);
    } else if (strcmp(mutex_name, "file") == 0) {
        enqueue(file_blocked_queue, current_process_id);
    }
    
    // Change state in memory to Blocked if it's currently loaded
    int start, end;
    get_process_bounds(current_process_id, &start, &end);
    if (start != -1) {
        if (memory[start + 1].value != NULL) free(memory[start + 1].value);
        memory[start + 1].value = strdup(STATE_BLOCKED);
    }

    // Force context switch
    current_process_id = -1;
}

int unblock_process(char *mutex_name) {
    int process_id = -1;
    
    if (strcmp(mutex_name, "userInput") == 0) {
        if (!is_empty(input_blocked_queue)) {
            process_id = dequeue(input_blocked_queue); // Remove from blocked queue
        }
    } else if (strcmp(mutex_name, "userOutput") == 0) {
        if (!is_empty(output_blocked_queue)) {
            process_id = dequeue(output_blocked_queue); // Remove from blocked queue
        }
    } else if (strcmp(mutex_name, "file") == 0) {
        if (!is_empty(file_blocked_queue)) {
            process_id = dequeue(file_blocked_queue); // Remove from blocked queue
        }
    }

    // Change state in memory to Ready if it's currently loaded
    int start, end;
    get_process_bounds(process_id, &start, &end);
    if (start != -1) {
        if (memory[start + 1].value != NULL) free(memory[start + 1].value);
        memory[start + 1].value = strdup(STATE_READY);
    }

    // Enqueue back to ready queue
    if (process_id != -1) {
        int start, end;
        get_process_bounds(process_id, &start, &end);
        if (start != -1) {
            if (memory[start + 1].value != NULL) free(memory[start + 1].value);
            memory[start + 1].value = strdup(STATE_READY);
        }

        // Enqueue back to ready queue
        enqueue(ready_queue[0], process_id);
    }
    
    return process_id;
}

// Returns the index (pid) of the process with the highest response ratio
// Returns -1 if no eligible process is found
int get_HRRN(int *remaining_pids, int remaining_count) {
    int    best_pid            = -1;
    double best_response_ratio = -1.0;

    for (int i = 0; i < remaining_count; i++) {
        int pid = remaining_pids[i];

        // Only consider processes that have already arrived
        if (LOAD_TIMES[pid] <= time) {
            int waiting_time    = time - LOAD_TIMES[pid];
            int burst_time      = instruction_counts[pid];
            double response_ratio = (double)(waiting_time + burst_time) / burst_time;

            if (response_ratio > best_response_ratio) {
                best_response_ratio = response_ratio;
                best_pid            = pid;
            }
        }
    }

    return best_pid;
}

void run_hrrn_scheduler() {
    int completed_processes = 0;

    // Track which processes are still remaining
    int remaining_pids[FILE_COUNT];
    int remaining_count = FILE_COUNT;
    for (int i = 0; i < FILE_COUNT; i++) remaining_pids[i] = i;

    while (completed_processes < FILE_COUNT) {

        // 1. Load any newly arrived processes into memory
        for (int i = 0; i < FILE_COUNT; i++) {
            if (LOAD_TIMES[i] == time) {
                if (TESTING_MODE) printf("[Time %d] Process %d arrived.\n", time, i);

                if (!try_load_process_into_memory(i, 0)) {
                    if (TESTING_MODE) printf("[Time %d] Failed to load Process %d (Memory Full).\n", time, i);
                }
                change_process_state(i, STATE_READY);
            }
        }

        // 2. Pick the highest response ratio process (HRRN is non-preemptive)
        if (current_process_id == -1) {
            int best_pid = get_HRRN(remaining_pids, remaining_count);

            if (best_pid != -1) {
                current_process_id = best_pid;

                // Swap it out of remaining list
                for (int i = 0; i < remaining_count; i++) {
                    if (remaining_pids[i] == best_pid) {
                        remaining_pids[i] = remaining_pids[--remaining_count];
                        break;
                    }
                }

                // Load from disk if swapped out
                int start, end;
                get_process_bounds(current_process_id, &start, &end);
                if (start == -1) try_load_process_into_memory(current_process_id, 1);

                change_process_state(current_process_id, STATE_RUNNING);
            }
        }

        // 3. Execute one instruction for the running process
        if (current_process_id != -1) {
            int start, end;
            get_process_bounds(current_process_id, &start, &end);

            if (start != -1) {
                int pc = atoi((char *)memory[start + 3].value);
                char *instruction = memory[start + 7 + pc].value;

                printf("[Time %d] Process %d executing [PC: %d]: %s\n", time, current_process_id, pc, instruction);
                print_process();
                parse_line(instruction);

                // Update PC
                pc++;
                char pc_str[16];
                snprintf(pc_str, 16, "%d", pc);
                if (memory[start + 3].value != NULL) memory[start + 3].value = strdup(pc_str);

                // Check if process finished all instructions
                if (pc >= instruction_counts[current_process_id]) {
                    if (TESTING_MODE) printf("[Time %d] Process %d Finished Execution.\n", time, current_process_id);
                    change_process_state(current_process_id, STATE_TERMINATED);

                    for (int i = start; i <= end; i++) {
                        if (memory[i].name)  free(memory[i].name);
                        if (memory[i].value) free(memory[i].value);
                        memory[i].name  = NULL;
                        memory[i].value = NULL;
                        memory[i].type  = 0;
                    }
                    compact_memory();

                    current_process_id = -1;
                    completed_processes++;
                }
            } else {
                current_process_id = -1;
            }
        } else {
            if (TESTING_MODE) printf("[Time %d] CPU Idle.\n", time);
        }

        // 4. Advance time
        advance_time();
    }

    if (TESTING_MODE) printf("--- HRRN Scheduling Complete at Time %d ---\n", time);
}

void run_rr_scheduler() {
    int completed_processes = 0;
    int current_quantum_used = 0;

    while (completed_processes < FILE_COUNT) {
        
        // 1. Check for New Process Arrivals at current time unit
        for (int i = 0; i < FILE_COUNT; i++) {
            if (LOAD_TIMES[i] == time) {
                if (TESTING_MODE) printf("[Time %d] Process %d arrived.\n", time, i);
                
                // Load into memory (try_looking_for_context = 0 since it's brand new)
                if (try_load_process_into_memory(i, 0)) {
                    change_process_state(i, STATE_READY);
                } else {
                    if (TESTING_MODE) printf("[Time %d] Failed to load Process %d (Memory Full).\n", time, i);
                }
                
                enqueue(ready_queue[0], i);
            }
        }

        // 2. Check if current process exhausted its time quantum
        if (current_process_id != -1 && current_quantum_used == RR_TIME_QUANTUM) {
            change_process_state(current_process_id, STATE_READY);
            enqueue(ready_queue[0], current_process_id);
            
            current_process_id = -1; // Deschedule
        }

        // 3. Schedule the next process from the Ready Queue if CPU is idle
        if (current_process_id == -1 && !is_empty(ready_queue[0])) {
            current_process_id = dequeue(ready_queue[0]);
            current_quantum_used = 0;

            // Ensure the scheduled process is in memory before running.
            // If it was swapped out, this function will pull its context back from disk.
            int start, end;
            get_process_bounds(current_process_id, &start, &end);
            
            if (start == -1) {
                try_load_process_into_memory(current_process_id, 1);
            }
            
            change_process_state(current_process_id, STATE_RUNNING);
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
                
                print_process(); // Print process state before executing, showing the next instruciton to run

                // Parse and execute the instruction
                parse_line(instruction);

                // Increment PC and Quantum tracking
                pc++;
                current_quantum_used++;

                // Update the PC in memory
                char pc_str[16];
                snprintf(pc_str, 16, "%d", pc);
                if (memory[start + 3].value != NULL) memory[start + 3].value = strdup(pc_str);

                // Check if the process has finished all its instructions
                if (pc >= instruction_counts[current_process_id]) {
                    if (TESTING_MODE) printf("[Time %d] Process %d Finished Execution.\n", time, current_process_id);
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
            if (TESTING_MODE) printf("[Time %d] CPU Idle.\n", time);
        }

        // 5. Advance time unit
        advance_time();
    }

    if (TESTING_MODE) printf("--- Scheduling Complete at Time %d ---\n", time);
}

void run_mlfq_scheduler() {
    int completed_processes = 0;
    int current_quantum_used = 0;
    int current_level = 0;

    while (completed_processes < FILE_COUNT) {

        // 1. Check for new process arrivals — always enter at Q0 (highest priority)
        for (int i = 0; i < FILE_COUNT; i++) {
            if (LOAD_TIMES[i] == time) {
                if (TESTING_MODE) printf("[Time %d] Process %d arrived → Q0.\n", time, i);

                if (try_load_process_into_memory(i, 0)) {
                    change_process_state(i, STATE_READY);
                } else {
                    if (TESTING_MODE) printf("[Time %d] Failed to load Process %d (Memory Full).\n", time, i);
                }

                enqueue(ready_queue[0], i);
            }
        }

        // 2. Schedule next process from the highest-priority non-empty queue
        if (current_process_id == -1) {
            for (int i = 0; i < 4; i++) {
                if (!is_empty(ready_queue[i])) {
                    current_process_id  = dequeue(ready_queue[i]);
                    current_level       = i;
                    current_quantum_used = 0;

                    // Restore from disk if it was swapped out
                    int start, end;
                    get_process_bounds(current_process_id, &start, &end);
                    if (start == -1) {
                        try_load_process_into_memory(current_process_id, 1);
                    }

                    if (TESTING_MODE) printf("[Time %d] Process %d scheduled from Q%d (quantum=%d).\n",
                                             time, current_process_id, current_level, MLFQ_TIME_QUANTUMS[current_level]);

                    change_process_state(current_process_id, STATE_RUNNING);
                    break;
                }
            }
        }

        // 3. Execute one instruction for the running process
        if (current_process_id != -1) {
            int start, end;
            get_process_bounds(current_process_id, &start, &end);

            if (start != -1) {
                int pc = atoi((char *)memory[start + 3].value);
                char *instruction = memory[start + 7 + pc].value;

                printf("[Time %d] Process %d executing [PC: %d] (Q%d): %s\n",
                       time, current_process_id, pc, current_level, instruction);
                print_process();
                parse_line(instruction);
                printf("Done executing instruction.\n");

                pc++;
                current_quantum_used++;

                // Update the PC in memory
                char pc_str[16];
                snprintf(pc_str, 16, "%d", pc);
                if (memory[start + 3].value != NULL) memory[start + 3].value = strdup(pc_str);

                // Check if process has finished all its instructions
                if (pc >= instruction_counts[current_process_id]) {
                    if (TESTING_MODE) printf("[Time %d] Process %d Finished Execution.\n", time, current_process_id);
                    change_process_state(current_process_id, STATE_TERMINATED);

                    for (int i = start; i <= end; i++) {
                        if (memory[i].name)  free(memory[i].name);
                        if (memory[i].value) free(memory[i].value);
                        memory[i].name  = NULL;
                        memory[i].value = NULL;
                        memory[i].type  = 0;
                    }
                    compact_memory();

                    current_process_id = -1;
                    completed_processes++;
                }
            } else {
                // Process vanished from memory bounds — force deschedule
                printf("[Time %d] Process %d vanished from memory bounds! Forcing deschedule.\n", time, current_process_id);
                current_process_id = -1;
            }
        } else {
            if (TESTING_MODE) printf("[Time %d] CPU Idle.\n", time);
        }

        // 4. If current process exhausted its quantum, demote it to the next lower queue
        if (current_process_id != -1 && current_quantum_used >= MLFQ_TIME_QUANTUMS[current_level]) {
            int next_level = (current_level < 3) ? current_level + 1 : 3;

            if (TESTING_MODE) printf("[Time %d] Process %d quantum expired → demoted Q%d to Q%d.\n",
                                     time, current_process_id, current_level, next_level);

            change_process_state(current_process_id, STATE_READY);
            enqueue(ready_queue[next_level], current_process_id);
            current_process_id = -1;
        }

        // 5. Advance time unit
        advance_time();
    }

    // Cleanup the local MLFQ queues
    for (int i = 0; i < 4; i++) free(ready_queue[i]);

    if (TESTING_MODE) printf("--- MLFQ Scheduling Complete at Time %d ---\n", time);
}