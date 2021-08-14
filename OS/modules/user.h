#ifndef USER_H
#define USER_H

#include <time.h>

#include "globalMacros.h"

typedef struct user {
  char name[12];
  char password[12];
  time_t lastAccess;
} User;

typedef struct session {
  char name[12];
  char cwd[TOKENLEN]; // Current Working directory
  time_t lastAccess;
} Session;


int getOldestSessionAccess(Session *activeSessions);
#endif
