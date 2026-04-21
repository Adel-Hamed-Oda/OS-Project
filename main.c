#include <stdio.h>
#include <stdlib.h>

#include "Includes.h"

// Define the global variables exactly once here!
MemoryWord *memory;
const char *FILE_NAMES[] = {"TestingFiles/Program_1.txt", "TestingFiles/Program_2.txt", "TestingFiles/Program_3.txt"};
const int LOAD_TIMES[] = {0, 1, 4};
int TESTING_MODE = 0;

void initialize();

void clean_up();

int main(int argc, char *argv[]) {
    initialize();

    if (argc > 1) {
        TESTING_MODE = atoi(argv[1]);
    }

    run_mlfq_scheduler();

    clean_up();

    return 0;
}

void initialize() {
    if (TESTING_MODE) setvbuf(stdout, NULL, _IONBF, 0);

    initialize_mutexes();
    initialize_memory();
    initialize_process_controller();
    initialize_scheduler();
}

void clean_up() {
    // theoritically this should not be needed since I tell the try_load_context function to not
    // look for a context file at the start, but just in case something weird happens I will always call
    // this at the end

    for (int i = 0; i < 100; i++) {
        char filename[256];
        snprintf(filename, sizeof(filename), "process_%d_context.txt", i);
        remove(filename); 
    }

    char command[512];

    snprintf(command, sizeof(command), "del /Q /F \"%s\\*.*\" 2>nul", USER_FILES_FOLDER_NAME);

    // Execute the terminal command
    system(command);
    
    printf("Cleanup complete!\n");
}