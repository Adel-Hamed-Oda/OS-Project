#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "includes.h"

void load_file_into_memory(char *filename);

const int MAX_LINE_LENGTH = 256;
const int NUMBER_OF_MEMORY_WORDS = 40;

//This is all just for testing and later on we will implement the schedulers in a much better way
//NOTE: I used to have a "last available memory index" but decided to remove this and just set any free memory to NULL
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    initialize(argc, argv);
}

void initialize(int argc, char *argv[]) {
    // every file will count as a process, and every file should be proceeded with the time it should
    // be loaded into the scheduler
    file_count = (argc - 1)/2;
    file_names = malloc(file_count * sizeof(char *));
    for (int i = 0; i < file_count; i++) {
        file_names[i] = argv[(2*i) + 1];
    }
    load_times = malloc(file_count * sizeof(int));
    for (int i = 0; i < file_count; i++) {
        load_times[i] = atoi(argv[(2*i) + 2]);
    }

    // initialize "memory"
    memory = malloc(NUMBER_OF_MEMORY_WORDS * sizeof(char *));
    for (int i = 0; i < NUMBER_OF_MEMORY_WORDS; i++) {
        memory[i] = NULL;
    }
}

// DEPRECATED: this is just for testing
void _load_file_into_memory(char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
    }

    char line[MAX_LINE_LENGTH];
    int line_count = 0;
    while (fgets(line, sizeof(line), file) && line_count < NUMBER_OF_MEMORY_WORDS) {
        memory[line_count] = strtok(strdup(line), "\n"); // remove newline character
        line_count++;
    }

    fclose(file);
}