#ifndef SESSION_H
#define SESSION_H

typedef struct session {
  char name[12];
  char cwd[TOKENLEN]; // Current Working directory
} Session;

#endif
