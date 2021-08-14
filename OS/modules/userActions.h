#ifndef USER_ACTIONS
#define USER_ACTIONS

#include <string.h>
#include <time.h>

#include "user.h"
#include "filesystem.h"
#include "fsDefinitions.h"
#include "globalMacros.h"

int createSession(Session *sesPtr, char *username, char *password);
int addUser(char *name, char *password);
int loadUserTable(User userTable[TOTALUSERS], iNodo *fileInode, Dir *userFile);
int updateUserInfo(char *username, char *password, time_t lastAccess);
int getOpenSesssionIndex(char *username);
void initSessions(void);

#endif
