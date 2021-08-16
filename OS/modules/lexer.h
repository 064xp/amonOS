#ifndef LEXER_H
#define LEXER_H

#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "globalMacros.h"
#include "commands.h"
#include "utils.h"
#include "user.h"

void separateCommand(char *command, int *argc, char (*argv)[TOKENLEN]);
int execute(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]);
void executeCommand(char *outputBuffer, Session *user, char *command);
void trim(char *buffer);
void separeParentPath(char *fullPath, char *parent, char *file);
char **mallocArgv(char *command);

#endif
