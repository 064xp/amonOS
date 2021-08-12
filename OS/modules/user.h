#ifndef USER_H
#define USER_H

#include "globalMacros.h"
#include "filesystem.h"
#include <time.h>

typedef struct user {
  char name[12];
  char password[12];
  time_t lastAccess;
} User;

int addUser(char *name, char *password);

#endif
