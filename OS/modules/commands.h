#ifndef COMMANDS_H
#define COMMANDS_H

#include "fsDefinitions.h"
#include "filesystem.h"
#include "user.h"
#include "userActions.h"
#include "globalMacros.h"

#define COMMANDSLEN 14

typedef struct command {
  char name[TOKENLEN];
  int (*function)(char*, Session*, int, char[][TOKENLEN]);
  char needsAuth;
  char *description;
} Command;

extern Command commands[COMMANDSLEN];

int ls(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]);
int touch(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]);
int mkdir(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]);
int rm(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]);
int cd(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]);
int pwd(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]);
int writef(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]);
int cat(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]);
int createUser(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]);
int login(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]);
int logout(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]);
int rr(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]);
int help(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]);

#endif
