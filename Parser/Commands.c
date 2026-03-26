#include <stdio.h>
#include <stdlib.h>

// W hena all the implementations
// NOTE, the arguments ALWAYS contain the name of the function: aka args[0] of print is "print"
// also count is the number of arguments (>= 1 because of the function name)

#include "Commands.h"

void cmd_semWait(char* args[], int count) {
    printf("gonna be swimmin, they're gonna be hatin\n");
}

void cmd_semSignal(char* args[], int count) {
    printf("end this miserable existence\n");
}

void cmd_assign(char* args[], int count) {
    printf("assigning value\n");
}

void cmd_print(char* args[], int count) {
    printf("printing value\n");
}

void cmd_writeFile(char* args[], int count) {
    printf("writing to file\n");
}
