#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Includes.h"

char** separate_by_spaces(char *input_string, int *count);
void semWait(char** args);
void semSignal(char** args);
void assign(char** args);
void print(char** args);
void writeFile(char** args);
void printFromTo(char** args);

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
    } else if (strcmp(command[0], "printFromTo") == 0) {
        printFromTo(command);
    } else {
        fprintf(stderr, "Error: Unknown command '%s'\n", command[0]);
    }
}

char** separate_by_spaces(char *input_string, int *count) {
    char **result = NULL;
    int i = 0;

    char *token = strtok(strdup(input_string), " "); //wtf is this? IDK, but it works
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
    int success = sem_wait_mutex(current_process_id, args[1]);

    if (!success) {
        block_process(args[1]);
    }
}

void semSignal(char** args) {
    sem_signal_mutex(current_process_id, args[1]);
    int unblocked_process_id = unblock_process(args[1]);

    if (unblocked_process_id != -1) {
        sem_wait_mutex(unblocked_process_id, args[1]);
    }
}

// args[1] should always be a variable
// args[2] can be another variable or input or readfile (I will ignore literals for now)
void assign(char** args) {
    char *var_name = args[1];
    char *value = args[2];

    if (strcmp(value, "input") == 0) {
        char *result = system_getInput();
        if (result != NULL) {
            system_setVar(var_name, result);
            free(result);
        }
    } else if (strcmp(value, "readFile") == 0) {
        char *file_name = system_getVar(args[3]); 
        
        if (file_name != NULL) {
            char *result = system_readFile(file_name);
            
            if (result != NULL) {
                system_setVar(var_name, result);
                free(result);
            }
        } else {
            fprintf(stderr, "Error: Variable '%s' not found for readFile.\n", args[3]);
        }
    } else {
        char *var_value = system_getVar(value);
        if (var_value != NULL) {
            system_setVar(var_name, var_value);
        }
    }
}

// args[1] can either be a variable or readfile (I will skip both literals and input for now)
void print(char** args) {
    char *value = args[1];

    if (strcmp(value, "readFile") == 0) {
        char *file_name = args[2];
        
        char *result = system_readFile(file_name);
        system_print(result);
    } else {
        system_print(system_getVar(value));
    }
}

// args[1] should be the filename variable
// args[2] can either be a variable, readFile or input, again literals will be skipped
void writeFile(char** args) {
    char *file_name_var = args[1];
    char *content_arg = args[2];
    char *content = NULL;

    if (strcmp(content_arg, "input") == 0) {
        content = system_getInput();
    } else if (strcmp(content_arg, "readFile") == 0) {
        char *file_name_2 = args[3];
        content = system_readFile(file_name_2);
        free(file_name_2);
    } else {
        content = system_getVar(content_arg);
    }

    char *file_name = system_getVar(file_name_var);
    system_writeFile(file_name, content);

    free(file_name);
}

// for now I assume they are always variables
void printFromTo(char** args) {
    char *var1 = args[1];
    char *var2 = args[2];

    int start = atoi(system_getVar(var1));
    int end = atoi(system_getVar(var2));

    system_printFromTo(start, end);
}