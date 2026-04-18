#ifndef SYSTEM_CALLS_H
#define SYSTEM_CALLS_H

char *system_readFile(char *filename);
void system_writeFile(char *filename, char *content);
void system_print(char *content);
void system_printFromTo(int start, int end);
void system_assign(char *var_name, char *value);
char *system_getInput();
char *system_getVar(char *var_name);
void system_setVar(char *var_name, char *value);

#endif