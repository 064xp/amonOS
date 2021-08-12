#ifndef USER_H
#define USER_H

#include "globalMacros.h"
#include <time.h>

typedef struct user {
  char name[12];
  char password[12];
  time_t lastAccess;
} User;

typedef struct session {
  char name[12];
  char cwd[TOKENLEN]; // Current Working directory
} Session;

#endif
