#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "Includes.h"
static void ensure_user_folder() {
    mkdir(USER_FILES_FOLDER_NAME);
}

// Builds a heap-allocated path like "userFolder/filename" — caller must free()
static char *build_path(const char *filename) {
    // +2 for the '/' separator and null terminator
    size_t len = strlen(USER_FILES_FOLDER_NAME) + 1 + strlen(filename) + 1;
    char *path = malloc(len);
    if (path == NULL) return NULL;
    snprintf(path, len, "%s/%s", USER_FILES_FOLDER_NAME, filename);
    return path;
}

char *system_readFile(char *filename) {
    if (filename == NULL) return NULL;

    ensure_user_folder();

    char *path = build_path(filename);
    if (path == NULL) {
        fprintf(stderr, "Error: Memory allocation failed building path.\n");
        return NULL;
    }

    FILE *file = fopen(path, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file '%s' for reading.\n", path);
        free(path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    if (file_size == 0) {
        fclose(file);
        free(path);
        char *empty = malloc(1);
        empty[0] = '\0';
        return empty;
    }

    char *content = malloc((file_size + 1) * sizeof(char));
    if (content == NULL) {
        fprintf(stderr, "Error: Memory allocation failed during readFile.\n");
        fclose(file);
        free(path);
        return NULL;
    }

    size_t bytes_read = fread(content, sizeof(char), file_size, file);
    content[bytes_read] = '\0';

    fclose(file);
    free(path);
    return content;
}

// Writes the provided string into a file (Overwrites existing content)
void system_writeFile(char *filename, char *content) {
    if (filename == NULL) return;

    ensure_user_folder();

    char *path = build_path(filename);
    if (path == NULL) {
        fprintf(stderr, "Error: Memory allocation failed building path.\n");
        return;
    }

    FILE *file = fopen(path, "w");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file '%s' for writing.\n", path);
        free(path);
        return;
    }

    fputs(content == NULL ? "(null)" : content, file);

    fclose(file);
    free(path);
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