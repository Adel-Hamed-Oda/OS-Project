#ifndef PUBLIC_DOMAIN_H
#define PUBLIC_DOMAIN_H

typedef struct MemoryWord {
    char* name;
    char* value;
    int type; // 0 = free, 1 = pcb, 2 = var, 3 = instruction
} MemoryWord;

// this tells the compiler that there is a variable called "memory" that is defined in Public Domain.c
extern char **memory;

// this tells the compiler that there is a variable called "filecount" that is defined in Public Domain.c
extern const int FILE_COUNT = 3;

// this tells the compiler that there is a variable called "filenames" that is defined in Public Domain.c
extern const char **FILE_NAMES = {"Program_1.txt", "Program_2.txt", "Program_3.txt"};

// this tells the compiler that there is a variable called "load_times" that is defined in Public Domain.c
extern const int *LOAD_TIMES = {0, 1, 4};

extern const int NUMBER_OF_MEMORY_WORDS = 40;

#endif