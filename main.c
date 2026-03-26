#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Parser/Parser.h"
#include "Scheduler/Scheduler.h"
#include "Process Controller/Process Controller.h"

void load_file_into_memory(char *filename);

const int MAX_LINE_LENGTH = 256;
const int NUMBER_OF_MEMORY_WORDS = 40;

char **memory;
int dynamic_memory_index; // this should be the last available memory word

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
    dynamic_memory_index = NUMBER_OF_MEMORY_WORDS - 1;

    load_file_into_memory(filename);

    //testing to ensure it's working
    for (int i = 0; i < NUMBER_OF_MEMORY_WORDS; i++) {
        printf("Memory[%d]: %s\n", i, memory[i]);
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