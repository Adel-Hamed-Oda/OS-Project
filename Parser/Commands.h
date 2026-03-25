#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdio.h>
#include <stdlib.h>

void cmd_semWait(char* args[], int count);
void cmd_semSignal(char* args[], int count);
void cmd_assign(char* args[], int count);
void cmd_print(char* args[], int count);
void cmd_writeFile(char* args[], int count);

#endif