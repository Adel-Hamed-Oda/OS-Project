#include <stdio.h>
#include <stdlib.h>

#include "includes.h"

void load_file_into_memory(char *filename);

const int MAX_LINE_LENGTH = 256;
const int NUMBER_OF_MEMORY_WORDS = 40;

//This is all just for testing and later on we will implement the schedulers in a much better way
//NOTE: I used to have a "last available memory index" but decided to remove this and just set any free memory to NULL
int main(int argc, char *argv[]) {
    initialize();
}

void initialize() {
    initialize_memory();
    initialize_process_controller(FILE_COUNT);
    initialize_scheduler();
}