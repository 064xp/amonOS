#ifndef USER_H
#define USER_H

#include "globalMacros.h"

typedef struct User {
  char name[12];
  char cwd[TOKENLEN]; // Current Working directory
} User;

#endif
