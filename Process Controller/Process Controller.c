#include <stdio.h>
#include <stdlib.h>

#include "Includes.h"

const int MAX_LINE_LENGTH = 256;
int total_processes = 0;
static char **instructions; // this is a simple process table to keep track of all processes, indexed by PID

void initialize_process_controller(int max_processes) {
    instructions = malloc(sizeof(char *) *max_processes);

    for (int i = 0; i < max_processes; i++) {
        create_new_process(FILE_NAMES[i]);
    }
}

void create_new_process(char *file_name) {
    FILE *file = fopen(file_name, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", file_name);
        return;
    }

    while (fgets(instructions[total_processes], MAX_LINE_LENGTH, file) != NULL) {
        size_t len = strlen(instructions[total_processes]);
        if (len > 0 && instructions[total_processes][len - 1] == '\n') {
            instructions[total_processes][len - 1] = '\0';
        }
    }
    
    total_processes++;
}

char *get_instructions_by_id(int id) {
    return instructions[id];
}