#include <stdio.h>
#include <stdlib.h>

#include "Includes.h"

const int MAX_LINE_LENGTH = 256;
int total_processes = 0;
static char **instructions; // this is a simple process table to keep track of all processes, indexed by PID
static int instruction_counts[FILE_COUNT]; // this keeps track of how many instructions each process has

void initialize_process_controller() {
    instructions = malloc(sizeof(char *) * FILE_COUNT);

    for (int i = 0; i < FILE_COUNT; i++) {
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
        instruction_counts[total_processes]++;
    }

    fclose(file);
    total_processes++;
}