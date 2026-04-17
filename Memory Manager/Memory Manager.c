#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Includes.h"

// why not use the .h file, well I don't want certain functions to be accessible outside of this file
int find_free_memory(int required_size);
int allocate_process(int pid, int start);
int try_swapping_out_processes(int required_size);
void load_context(int pid, int start);
void save_context(int pid);
void compact_memory();
void get_process_bounds(int pid, int *start, int *end);


void initialize_memory() {
    memory = malloc(sizeof(MemoryWord) * NUMBER_OF_MEMORY_WORDS);

    for (int i = 0; i < NUMBER_OF_MEMORY_WORDS; i++) {
        memory[i].name = NULL;
        memory[i].value = NULL;
        memory[i].type = 0; // mark all memory words as free
    }
}

int try_load_process_into_memory(int pid, int try_looking_for_context) {
    int required_size = 4 + 3 + instruction_counts[pid]; // 4 for PCB, 3 for variables, rest for instructions
    
    // Attempt to load from context first if requested
    if (try_looking_for_context) {
        char filename[256];
        snprintf(filename, sizeof(filename), "process_%d_context.txt", pid);
        FILE *file = fopen(filename, "r");
        
        if (file != NULL) {
            int start = find_free_memory(required_size);
            if (start == -1) {
                if (try_swapping_out_processes(required_size) == -1) {
                    fclose(file);
                    return -1;
                }
            }

            // Space is guaranteed now, grab the correct index and load
            load_context(pid, find_free_memory(required_size)); 
            fclose(file);
            return 0; // Fixed double semicolon
        }
    } 
    
    // If context was not requested OR the context file didn't exist, allocate new:
    int start = find_free_memory(required_size);
    if (start == -1) {
        if (try_swapping_out_processes(required_size) == -1) {
            return -1;
        }
        start = find_free_memory(required_size);
    }
    
    allocate_process(pid, start);
    return 0;
}
    
int allocate_process(int pid, int start) {
    // We need 4 words for PCB + 3 words for Variables + N words for instructions
    int total_required = 4 + 3 + instruction_counts[pid];

    // 1. Setup PCB (id)
    char *pid_str = malloc(16 * sizeof(char));
    snprintf(pid_str, 16, "%d", pid);
    memory[start].name = "id";
    memory[start].value = pid_str; // Assigned formatted string
    memory[start].type = 1; 

    // 2. Setup PCB (state)
    memory[start + 1].name = "state";
    memory[start + 1].value = "0"; // Stored as string, not raw integer 0
    memory[start + 1].type = 1; 

    // 3. Setup PCB (bounds)
    int end_index = start + total_required - 1;
    char *bounds_str = malloc(32 * sizeof(char));
    snprintf(bounds_str, 32, "%d-%d", start, end_index);
    
    memory[start + 2].name = "bounds";
    memory[start + 2].value = bounds_str; // Assigned formatted string
    memory[start + 2].type = 1; 

    // 4. Setup PCB (pc)
    memory[start + 3].name = "pc";
    memory[start + 3].value = "0"; // Stored as string
    memory[start + 3].type = 1; 

    // 5. Reserve 3 Variable Spaces
    for (int i = 0; i < 3; i++) {
        memory[start + 4 + i].name = NULL;
        memory[start + 4 + i].value = NULL;
        memory[start + 4 + i].type = 2; // mark this memory word as a variable
    }

    // 6. Store Instructions
    for (int i = 0; i < instruction_counts[pid]; i++) {
        memory[start + 4 + 3 + i].name = NULL;
        memory[start + 4 + 3 + i].value = instructions[pid][i];
        memory[start + 4 + 3 + i].type = 3; // mark this memory word as an instruction
    }

    return start; // Successfully allocated, returning start index
}

void change_process_state(int pid, const char* state) {
    int start, end;
    get_process_bounds(pid, &start, &end);
    if (start != -1) {
        if (memory[start + 1].value != NULL) free(memory[start + 1].value);
        memory[start + 1].value = strdup(state); // Update state as string
    }
}

void compact_memory() {
    int free_index = 0;

    // 1. Compact memory by sliding all non-free words to the left
    for (int i = 0; i < NUMBER_OF_MEMORY_WORDS; i++) {
        if (memory[i].type != 0) { // If this memory word is not free
            if (i != free_index) { 
                memory[free_index] = memory[i]; // Move it
                
                // Clear the old position
                memory[i].name = NULL;
                memory[i].value = NULL;
                memory[i].type = 0;
            }
            free_index++;
        }
    }

    // 2. Update the "bounds" variable for each process in its new location
    for (int i = 0; i < free_index; i++) {
        if (memory[i].type == 1 && memory[i].name != NULL && strcmp(memory[i].name, "bounds") == 0) {
            
            int new_start = i - 2;
            int old_start, old_end;
            
            // sscanf reads the integers safely without modifying the source string
            sscanf((char *)memory[i].value, "%d-%d", &old_start, &old_end);
            
            int process_size = old_end - old_start;
            int new_end = new_start + process_size;

            char *new_bounds = malloc(32 * sizeof(char));
            snprintf(new_bounds, 32, "%d-%d", new_start, new_end);

            memory[i].value = new_bounds;
        }
    }
}

int find_free_memory(int required_size) {
    int free_count = 0;

    for (int i = 0; i < NUMBER_OF_MEMORY_WORDS; i++) {
        if (memory[i].type == 0) { // If this memory word is free
            free_count++;
            if (free_count == required_size) { // If we've found enough contiguous free memory
                return i - required_size + 1; // Return the starting index of the free block
            }
        } else {
            free_count = 0; // Reset count if we hit an occupied memory word
        }
    }

    return -1; // No sufficient free block found
}

char *get_variable(char *var_name) {
    int start, end;
    get_process_bounds(current_process_id, &start, &end);

    if (start == -1 || end == -1) {
        return NULL; // Process bounds not found
    }

    for (int i = start; i <= end; i++) {
        if (memory[i].type == 2 && memory[i].name != NULL && strcmp(memory[i].name, var_name) == 0) {
            return memory[i].value; // Return the value of the variable
        }
    }
    return NULL; // Variable not found
}

void set_variable(char *var_name, char *value) {
    int start, end;
    get_process_bounds(current_process_id, &start, &end);

    if (start == -1 || end == -1) {
        fprintf(stderr, "Error: Process bounds not found\n");
        return;
    }

    for (int i = start; i <= end; i++) {
        if (memory[i].type == 2 && memory[i].name != NULL && strcmp(memory[i].name, var_name) == 0) {
            memory[i].value = value; // Update the value of the existing variable
            return;
        }
    }

    // If variable does not exist, find a free memory word to store it
    int free_index = find_free_memory(1);
    if (free_index != -1) {
        memory[free_index].name = var_name;
        memory[free_index].value = value;
        memory[free_index].type = 2; // Mark this memory word as a variable
    } else {
        fprintf(stderr, "Error: Not enough memory to store variable '%s'\n", var_name);
    }
}

void get_process_bounds(int pid, int *start, int *end) {
    int process_location = -1;

    for (int i = 0; i < NUMBER_OF_MEMORY_WORDS; i++) {
        if (memory[i].type == 1 && memory[i].name != NULL && 
            strcmp(memory[i].name, "id") == 0 && atoi((char *)memory[i].value) == pid) {
            process_location = i;
            break;
        }
    }

    if (process_location == -1) {
        *start = -1;
        *end = -1;
        return; // Process not found
    }

    char *bounds = (char *)memory[process_location + 2].value;
    
    // CRITICAL FIX: sscanf extracts the numbers without destroying the original string.
    // strtok inserts '\0' characters, which was destroying your memory block!
    sscanf(bounds, "%d-%d", start, end);
}

void save_context(int pid) {
    int start, end;
    get_process_bounds(pid, &start, &end);
    if (start == -1 || end == -1) {
        fprintf(stderr, "Error: Process bounds not found for PID %d\n", pid);
        return;
    }
    
    char *filename = malloc(256 * sizeof(char));
    snprintf(filename, 256, "process_%d_context.txt", pid);
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error: Failed to open file for saving context\n");
        free(filename);
        return;
    }

    for (int i = start; i <= end; i++) {
        if (memory[i].type != 0) { 
            // CRITICAL FIX: explicitly handle NULL values to prevent segmentation faults
            const char* s_name = memory[i].name ? memory[i].name : "(null)";
            const char* s_value = memory[i].value ? memory[i].value : "(null)";
            
            fprintf(file, "%s:%s:%d\n", s_name, s_value, memory[i].type);

            memory[i].name = NULL; // Clear the memory word
            memory[i].value = NULL;
            memory[i].type = 0; // Mark it as free
        }
    }

    compact_memory(); // Compact memory after saving context to free up space

    fclose(file);
    free(filename);
}

void load_context(int pid, int start) {
    char *filename = malloc(256 * sizeof(char));
    snprintf(filename, 256, "process_%d_context.txt", pid);
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Failed to open file for loading context\n");
        free(filename);
        return;
    }

    char line[256];
    int current_index = start;

    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = '\0';

        char *name_token = strtok(line, ":");
        char *value_token = strtok(NULL, ":");
        char *type_token = strtok(NULL, ":");

        if (name_token != NULL && value_token != NULL && type_token != NULL) {
            
            if (strcmp(name_token, "(null)") == 0) {
                memory[current_index].name = NULL;
            } else {
                memory[current_index].name = strdup(name_token); 
            }

            if (strcmp(value_token, "(null)") == 0) {
                memory[current_index].value = NULL;
            } else {
                memory[current_index].value = strdup(value_token);
            }

            memory[current_index].type = atoi(type_token);
            current_index++; 
        }
    }

    if (current_index > start + 2 && 
        memory[start + 2].name != NULL && 
        strcmp((char *)memory[start + 2].name, "bounds") == 0) {
        
        int new_end = current_index - 1; 
        char *new_bounds = malloc(32 * sizeof(char));
        snprintf(new_bounds, 32, "%d-%d", start, new_end);
        
        if (memory[start + 2].value != NULL) {
            free(memory[start + 2].value); 
        }
        
        memory[start + 2].value = new_bounds;
    }

    fclose(file);
    remove(filename); 
    free(filename);
}

int try_swapping_out_processes(int required_size) {
    if (required_size > NUMBER_OF_MEMORY_WORDS) {
        fprintf(stderr, "Error: Process requires %d words, which exceeds max memory of %d.\n", 
                required_size, NUMBER_OF_MEMORY_WORDS);
        return -1;
    }

    while (find_free_memory(required_size) == -1) {
        
        int largest_pid = -1;
        int max_size = -1;

        for (int i = 0; i < NUMBER_OF_MEMORY_WORDS; i++) {
            if (memory[i].type == 1 && memory[i].name != NULL && strcmp(memory[i].name, "id") == 0) {
                int pid = atoi((char *)memory[i].value);
                
                int start, end;
                get_process_bounds(pid, &start, &end);
                
                if (start != -1 && end != -1) {
                    int size = end - start + 1;
                    
                    if (size > max_size) {
                        max_size = size;
                        largest_pid = pid;
                    }
                }
            }
        }

        if (largest_pid == -1) {
            fprintf(stderr, "Error: No processes left to swap out, but still need %d words.\n", required_size);
            return -1;
        }

        printf("Swapping out process %d (size: %d words) to free memory...\n", largest_pid, max_size);
        save_context(largest_pid); 
    }

    return 0; 
}