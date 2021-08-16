#ifndef USER_ACTIONS
#define USER_ACTIONS

#include <string.h>
#include <time.h>

#include "globalMacros.h"
#include "fsDefinitions.h"
#include "filesystem.h"
#include "lexer.h"
#include "user.h"
#include "utils.h"
#include "IPCtypes.h"

int createSession(Session *sesPtr, char *username, char *password);
int addUser(char *name, char *password);
int loadUserTable(User userTable[TOTALUSERS], iNodo *fileInode, Dir *userFile);
int updateUserInfo(char *username, char *password, time_t lastAccess);
int getOpenSesssionIndex(char *username);
void initSessions(void);
void executeAuthenticated(RequestPacket *request, ResponsePacket *response);
int deleteSession(Session *sesPtr);

#endif
