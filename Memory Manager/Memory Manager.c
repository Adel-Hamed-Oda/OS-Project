#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Includes.h"

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
    
    if (try_looking_for_context) {
        char filename[256];
        snprintf(filename, sizeof(filename), "process_%d_context.txt", pid);
        FILE *file = fopen(filename, "r");
        if (file != NULL) {
            int start = find_free_memory(4 + 3 + instruction_counts[pid]);
            if (start == -1) {
                try_swapping_out_processes(required_size);
            }

            load_context(pid, find_free_memory(required_size)); // Try to load context at any free memory location
            fclose(file);
            return 0;;
        }
    } else {
        int start = find_free_memory(required_size);
        if (start == -1) {
            try_swapping_out_processes(required_size);
        }
        allocate_process(pid, start);
    }
    
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
    // The end bound is start + total required elements - 1
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
    // Corrected loop limit from 4 to 3 to match your memory allocation logic
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
        // Find the bounds memory word
        if (memory[i].type == 1 && memory[i].name != NULL && strcmp(memory[i].name, "bounds") == 0) {
            
            // The new starting index is exactly 2 positions before "bounds" 
            // (because PCB order is: id, state, bounds, pc)
            int new_start = i - 2;

            // Parse the old bounds string to determine the length/size of the process
            int old_start, old_end;
            
            // sscanf reads the integers out of the "start-end" string
            sscanf((char *)memory[i].value, "%d-%d", &old_start, &old_end);
            
            int process_size = old_end - old_start;
            int new_end = new_start + process_size;

            // Create the new bounds string formatted as "new_start-new_end"
            char *new_bounds = malloc(32 * sizeof(char));
            snprintf(new_bounds, 32, "%d-%d", new_start, new_end);

            // Update the memory value to the new string
            // Note: If memory[i].value was previously malloc'd, you should free() it here first!
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
        if (memory[i].type == 2 && strcmp(memory[i].name, var_name) == 0) {
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
        if (memory[i].type == 2 && strcmp(memory[i].name, var_name) == 0) {
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
        if (memory[i].type == 1 && strcmp(memory[i].name, "id") == 0 && atoi(memory[i].value) == pid) {
            process_location = i;
            break;
        }
    }

    if (process_location == -1) {
        *start = -1;
        *end = -1;
        return; // Process not found
    }

    char *bounds = memory[process_location + 2].value;
    *start = atoi(strtok(bounds, "-"));
    *end = atoi(strtok(NULL, "-"));
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
        if (memory[i].type != 0) { // Only save non-free memory words
            fprintf(file, "%s:%s:%d\n", memory[i].name, memory[i].value, memory[i].type);
        }
    }

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

    // Read the file line by line
    while (fgets(line, sizeof(line), file) != NULL) {
        // Remove the newline character at the end of the line
        line[strcspn(line, "\n")] = '\0';

        // Parse the line using the ':' delimiter
        char *name_token = strtok(line, ":");
        char *value_token = strtok(NULL, ":");
        char *type_token = strtok(NULL, ":");

        if (name_token != NULL && value_token != NULL && type_token != NULL) {
            
            // 1. Restore the 'name'
            if (strcmp(name_token, "(null)") == 0) {
                memory[current_index].name = NULL;
            } else {
                memory[current_index].name = strdup(name_token); // Dynamically allocate the string
            }

            // 2. Restore the 'value'
            if (strcmp(value_token, "(null)") == 0) {
                memory[current_index].value = NULL;
            } else {
                memory[current_index].value = strdup(value_token);
            }

            // 3. Restore the 'type'
            memory[current_index].type = atoi(type_token);

            current_index++; // Move to the next memory word
        }
    }

    // 4. Update the "bounds" memory word to reflect the new location
    // The PCB layout is: id (start), state (start+1), bounds (start+2)
    if (current_index > start + 2 && 
        memory[start + 2].name != NULL && 
        strcmp(memory[start + 2].name, "bounds") == 0) {
        
        int new_end = current_index - 1; // The last index we wrote to
        char *new_bounds = malloc(32 * sizeof(char));
        snprintf(new_bounds, 32, "%d-%d", start, new_end);
        
        // Free the old bounds string loaded from the file
        if (memory[start + 2].value != NULL) {
            free(memory[start + 2].value); 
        }
        
        // Assign the newly calculated bounds
        memory[start + 2].value = new_bounds;
    }

    fclose(file);
    
    // Optional: Delete the context file from the disk once it is safely loaded
    remove(filename); 
    
    free(filename);
}