#ifndef COMMANDS_H
#define COMMANDS_H

#include "filesystem.h"
#include "lexer.h"
#include "user.h"
#include "globalMacros.h"

int ls(char *outputBuffer, User *user, int argc, char argv[][TOKENLEN]);
int touch(char *outputBuffer, User *user, int argc, char argv[][TOKENLEN]);
int mkdir(char *outputBuffer, User *user, int argc, char argv[][TOKENLEN]);
int rm(char *outputBuffer, User *user, int argc, char argv[][TOKENLEN]);
int cd(char *outputBuffer, User *user, int argc, char argv[][TOKENLEN]);
int pwd(char *outputBuffer, User *user, int argc, char argv[][TOKENLEN]);
int writef(char *outputBuffer, User *user, int argc, char argv[][TOKENLEN]);
int cat(char *outputBuffer, User *user, int argc, char argv[][TOKENLEN]);

#endif
