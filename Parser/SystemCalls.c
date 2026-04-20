#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Includes.h"

// Reads an entire file and returns it as a dynamically allocated string
char *system_readFile(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file '%s' for reading.\n", filename);
        return NULL;
    }

    // 1. Jump to the end of the file to find out how many bytes it is
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    
    // 2. Rewind back to the beginning of the file
    rewind(file); 

    // 3. Allocate memory for the string (+1 for the null terminator '\0')
    char *content = malloc((file_size + 1) * sizeof(char));
    if (content == NULL) {
        fprintf(stderr, "Error: Memory allocation failed during readFile.\n");
        fclose(file);
        return NULL;
    }

    // 4. Read the entire file into our allocated string
    size_t bytes_read = fread(content, sizeof(char), file_size, file);
    content[bytes_read] = '\0'; // Properly terminate the string, removing

    fclose(file);
    
    // Note: The memory manager or process that requested this must free() it later!
    return strcmp(content, "(null)") == 0 ? NULL : content;
}

// Writes the provided string into a file (Overwrites existing content)
void system_writeFile(char *filename, char *content) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file '%s' for writing.\n", filename);
        return;
    }

    fputs(content == NULL ? "(null)" : content, file);

    fclose(file);
}

void system_print(char *content) {
    printf("%s\n", content);
}

void system_printFromTo(int start, int end) {
    for (int i = start; i <= end; i++) {
        printf("%d ", i);
    }

    printf("\n");
}

void system_assign(char *var_name, char *value) {
    memory_set_variable(var_name, value);
}

char *system_getInput() {
    char *input = malloc(sizeof(char) * MAX_LINE_LENGTH);
    if (input == NULL) return NULL;
    
    if (fgets(input, MAX_LINE_LENGTH, stdin) != NULL) {
        input[strcspn(input, "\n")] = '\0';
    }
    
    return input;
}

char *system_getVar(char *var_name) {
    return memory_get_variable(var_name);
}

void system_setVar(char *var_name, char *value) {
    memory_set_variable(var_name, value);
}