#ifndef COMMANDS_H
#define COMMANDS_H

#include "filesystem.h"
#include "user.h"

int ls(char *outputBuffer, User user, int argc, char argv[][TOKENLEN]);

#endif
