#ifndef USER_H
#define USER_H

typedef struct User {
  char name[12];
  char cwd[128]; // Current Working directory
} User;

#endif
