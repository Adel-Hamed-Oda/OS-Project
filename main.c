#include <stdio.h>
#include <stdlib.h>

#include "Includes.h"

void initialize();

const int MAX_LINE_LENGTH = 256;

//This is all just for testing and later on we will implement the schedulers in a much better way
//NOTE: I used to have a "last available memory index" but decided to remove this and just set any free memory to NULL
int main(void) {
    initialize();

    run_round_robin_scheduler();
}

void initialize() {
    initialize_memory();
    initialize_process_controller();
    initialize_scheduler();
}