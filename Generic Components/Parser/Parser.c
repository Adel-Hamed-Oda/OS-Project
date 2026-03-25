#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Parser.h"
#include "Commands.h"

const int MAX_LINE_LENGTH = 256;

void parse_file(char *filename) {
    FILE *file = fopen(filename, "r");

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        parse_line(line);
    }

    fclose(file);
}

void parse_line(char *line) {
    int count = 0;

    char** command = separate_by_spaces(line, &count);

    if (command == NULL || count == 0) return;

    if (strcmp(command[0], "assign") == 0) {
        //call assign
    } else if (strcmp(command[0], "print") == 0) {
        //call print
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