#include <stdio.h>
#include <stdlib.h>

#include "Includes.h"

// Define the global variables exactly once here!
MemoryWord *memory;
const char *FILE_NAMES[] = {"TestingFiles/Program_1.txt", "TestingFiles/Program_2.txt", "TestingFiles/Program_3.txt"};
const int LOAD_TIMES[] = {0, 1, 4};

void initialize();

int main(void) {
    initialize();

    run_round_robin_scheduler();
    return 0;
}

void initialize() {
    initialize_memory();
    initialize_process_controller();
    initialize_scheduler();
}