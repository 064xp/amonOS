#include "user.h"

int addUser(char *name, char *password){
  User userTable[TOTALUSERS];
  Dir userFile;
  iNodo fileInode;
  int i;

  userFile = namei("/etc/user", NULL);
  if(userFile.iNodo == -1){
    return 1;
  }
  getInode(&fileInode, userFile.iNodo);

  // Tabla de usuarios llena
  if(fileInode.size / sizeof(User) == TOTALUSERS){
    return 2;
  }

  getBlock(userTable, fileInode.contentTable[0]);

  for(i=0; i<TOTALUSERS; i++){
    if(userTable[i].name[0] == 0){
      strcpy(userTable[i].name, name);
      strcpy(userTable[i].password, password);
      userTable[i].lastAccess = getCurrentTime();
      break;
    }
  }

  fileInode.size += sizeof(User);
  writeBlock(userTable, fileInode.contentTable[0], 1024);
  writeInode(&fileInode, userFile.iNodo);

  return 0;
}
