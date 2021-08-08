#ifndef LEXER_H
#define LEXER_H

#include <string.h>
#include <stdio.h>
#include <string.h>

#include "globalMacros.h"
#include "commands.h"

typedef struct command {
  char name[TOKENLEN];
  int (*function)(char*, int, char[][TOKENLEN]);
} Command;

void separateCommand(char *command, int *argc, char argv[][TOKENLEN]);
int execute(char *outputBuffer, int argc, char argv[][TOKENLEN]);
void executeCommand(char *outputBuffer, char *command);

#endif
