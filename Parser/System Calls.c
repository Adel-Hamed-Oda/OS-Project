#include <stdio.h>
#include <stdlib.h>

#include "Includes.h"

int MAX_LINE_LENGTH = 256;

char *readFile(char *filename) {

}

void writeFile(char *filename, char *content) {

}

void print(char *content) {

}

void printFromTo(int start, int end) {

}

void assign(char *var_name, char *value) {

}

char *getInput() {
    char *input = malloc(sizeof(char) * MAX_LINE_LENGTH);
    if (input == NULL) return NULL;
    fgets(input, MAX_LINE_LENGTH, stdin);
    return input;
}

char *getVar(char *var_name) {
    
}