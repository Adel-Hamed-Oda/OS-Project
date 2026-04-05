#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "includes.h"

void parse_line(char *line) {
    int count = 0;

    char** command = separate_by_spaces(line, &count);

    if (command == NULL || count == 0) return;

    // akeed there is a better way but IDC, bruteforce it is
    if (strcmp(command[0], "semWait") == 0) {
        semWait(command);
    } else if (strcmp(command[0], "semSignal") == 0) {
        semSignal(command);
    } else if (strcmp(command[0], "assign") == 0) {
        assign(command);
    } else if (strcmp(command[0], "print") == 0) {
        print(command);
    } else if (strcmp(command[0], "writeFile") == 0) {
        writeFile(command);
    }
}

char** separate_by_spaces(char *input_string, int *count) {
    char **result = NULL;
    int i = 0;

    char *token = strtok(input_string, " "); //wtf is this? IDK, but it works
    while (token != NULL) {
        result = realloc(result, (i + 1) * sizeof(char *));

        result[i] = strdup(token); // copy paste
        i++;

        token = strtok(NULL, " ");
    }

    *count = i;
    return result;
}

void semWait(char** args) {

}

void semSignal(char** args) {

}

void assign(char** args) {

}

void print(char** args) {

}

void writeFile(char** args) {

}