#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Includes.h"

const int MAX_LINE_LENGTH = 256;
int total_processes = 0;
char ***instructions; // this is a simple process table to keep track of all processes, indexed by PID
int instruction_counts[FILE_COUNT]; // this keeps track of how many instructions each process has

void create_new_process(char *file_name);

void initialize_process_controller() {
    instructions = malloc(sizeof(char *) * FILE_COUNT);

    for (int i = 0; i < FILE_COUNT; i++) {
        create_new_process(strdup(FILE_NAMES[i]));
    }
}

void create_new_process(char *file_name) {
    FILE *file = fopen(file_name, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", file_name);
        return;
    }

    int count = 0;
    while (fgets(instructions[total_processes][count], MAX_LINE_LENGTH, file) != NULL) {
        size_t len = strlen(instructions[total_processes][count]);
        if (len > 0 && instructions[total_processes][count][len - 1] == '\n') {
            instructions[total_processes][count][len - 1] = '\0';
        }
        instruction_counts[total_processes]++;
    }

    fclose(file);
    total_processes++;
}