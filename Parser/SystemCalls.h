#ifndef SYSTEM_CALLS_H
#define SYSTEM_CALLS_H

char *readFile(char *filename);
void writeFile(char *filename, char *content);
void print(char *content);
void printFromTo(int start, int end);
void assign(char *var_name, char *value);
char *getInput();
char *getVar(char *var_name);

#endif