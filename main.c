#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Parser.h"
#include "Scheduler.h"
#include "Process Controller.h"

void load_file_into_memory(char *filename);

const int MAX_LINE_LENGTH = 256;
const int NUMBER_OF_MEMORY_WORDS = 40;
const int NUMBER_OF_SS_WORDS = 9999; // virtually infinite

char **memory;

//This is all just for testing and later on we will implement the schedulers in a much better way
//NOTE: I used to have a "last available memory index" but decided to remove this and just set any free memory to NULL
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    char *filename = argv[1];

    // initialize "memory"
    memory = malloc(NUMBER_OF_MEMORY_WORDS * sizeof(char *));
    for (int i = 0; i < NUMBER_OF_MEMORY_WORDS; i++) {
        memory[i] = NULL;
    }

    load_file_into_memory(filename);

    //execute it line by line
    for (int i = 0; i < NUMBER_OF_MEMORY_WORDS; i++) {
        if (memory[i] != NULL) {
            parse_line(memory[i]);
        }
    }
}

void load_file_into_memory(char *filename) {
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