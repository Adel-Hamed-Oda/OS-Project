#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Includes.h"

// Do not use "static" here, otherwise other files can't use these variables!
char ***instructions;
int instruction_counts[FILE_COUNT];

int total_processes = 0;

void initialize_process_controller() {
    // Allocate space for the number of processes
    instructions = malloc(sizeof(char **) * FILE_COUNT);

    for (int i = 0; i < FILE_COUNT; i++) {
        create_new_process((char *)FILE_NAMES[i]);
    }
}

void create_new_process(char *file_name) {
    FILE *file = fopen(file_name, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", file_name);
        return;
    }

    // Allocate space for up to 50 instructions per process
    instructions[total_processes] = malloc(sizeof(char *) * 50);
    instruction_counts[total_processes] = 0;

    char temp_line[MAX_LINE_LENGTH];

    while (fgets(temp_line, MAX_LINE_LENGTH, file) != NULL) {
        size_t len = strlen(temp_line);
        if (len > 0 && temp_line[len - 1] == '\n') {
            temp_line[len - 1] = '\0';
        }
        
        // Copy the string safely into the instructions array
        instructions[total_processes][instruction_counts[total_processes]] = strdup(temp_line);
        instruction_counts[total_processes]++;
    }

    fclose(file);
    total_processes++;
}