#ifndef COMMANDS_H
#define COMMANDS_H

#include "filesystem.h"
#include "lexer.h"
#include "session.h"
#include "globalMacros.h"

int ls(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]);
int touch(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]);
int mkdir(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]);
int rm(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]);
int cd(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]);
int pwd(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]);
int writef(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]);
int cat(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]);

#endif
