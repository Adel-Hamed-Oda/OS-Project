#include <stdio.h>
#include <stdlib.h>

#include "Includes.h"

int MAX_LINE_LENGTH = 256;

char *system_readFile(char *filename) {
    return NULL;
}

void system_writeFile(char *filename, char *content) {

}

void system_print(char *content) {

}

void system_printFromTo(int start, int end) {

}

void system_assign(char *var_name, char *value) {

}

char *system_getInput() {
    char *input = malloc(sizeof(char) * MAX_LINE_LENGTH);
    if (input == NULL) return NULL;
    fgets(input, MAX_LINE_LENGTH, stdin);
    return input;
}

char *system_getVar(char *var_name) {
    return NULL;
}