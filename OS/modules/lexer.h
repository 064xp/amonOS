#ifndef LEXER_H
#define LEXER_H

#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "globalMacros.h"
#include "commands.h"
#include "user.h"

typedef struct command {
  char name[TOKENLEN];
  int (*function)(char*, User*, int, char[][TOKENLEN]);
} Command;

void separateCommand(char *command, int *argc, char argv[][TOKENLEN]);
int execute(char *outputBuffer, User *user, int argc, char argv[][TOKENLEN]);
void executeCommand(char *outputBuffer, User *user, char *command);
void trim(char *buffer);
void separeParentPath(char *fullPath, char *parent, char *file);
void prepend(char* s, const char* t);

#endif
