#ifndef PUBLIC_DOMAIN_H
#define PUBLIC_DOMAIN_H

#define MAX_LINE_LENGTH 256
#define FILE_COUNT 3
#define NUMBER_OF_MEMORY_WORDS 40

typedef struct MemoryWord {
    char* name;
    char* value;
    int type; // 0 = free, 1 = pcb, 2 = var, 3 = instruction
} MemoryWord;

extern MemoryWord *memory;
extern const char *FILE_NAMES[];
extern const int LOAD_TIMES[];
extern int TESTING_MODE;

#endif