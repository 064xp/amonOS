#include "userActions.h"

Session activeSessions[MAX_SESSIONS];

/*
  Retvals
    0: Exitoso
    1: Error al leer la tabla de usuarios
    2: Tabla de usuarios llena
    3: Nombre de usuario ocupado
*/
int addUser(char *username, char *password){
  User userTable[TOTALUSERS];
  iNodo fileInode;
  Dir userFile;
  int i;

  if(loadUserTable(userTable, &fileInode, &userFile))
    return 1;

  // Tabla de usuarios llena
  if(fileInode.size / sizeof(User) == TOTALUSERS){
    return 2;
  }
  // Checar si ya esta ocupado ese username
  for(i=0; i<TOTALUSERS; i++){
    if(strcmp(username, userTable[i].name) == 0){
      return 3;
    }
  }

  // TODO: Manejar si esta llena la tabla de usuarios
  // Checar por un lugar libre
  for(i=0; i<TOTALUSERS; i++){
    if(userTable[i].name[0] == 0){
      strcpy(userTable[i].name, username);
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

/*
  Retvals
    0: Exitoso
    1: Error al leer la tabla de usuarios
    2: Usuario no encontrado
*/
int updateUserInfo(char *username, char *password, time_t lastAccess){
  User userTable[TOTALUSERS];
  iNodo fileInode;
  int i;

  if(loadUserTable(userTable, &fileInode, (Dir *)NULL))
    return 1;

  for(i=0; i<=TOTALUSERS; i++){
    if(i == TOTALUSERS)
      return 2;
    if(strcmp(username, userTable[i].name) == 0){
      strcpy(userTable[i].password, password);
      userTable[i].lastAccess = lastAccess;
      break;
    }
  }

  writeBlock(userTable, fileInode.contentTable[0], 1024);

  return 0;
}

int loadUserTable(User userTable[TOTALUSERS], iNodo *fileInode, Dir *userFile){
  *userFile = namei("/etc/user", NULL);
  if(userFile->iNodo == -1){
    return 1;
  }
  getInode(fileInode, userFile->iNodo);
  getBlock(userTable, fileInode->contentTable[0]);

  return 0;
}

/*
  Retvals
    0: Exitoso
    1: Error al leer la tabla de usuarios
    2: Usuario no encontrado / Credenciales incorrectas
*/
int login(Session *sesPtr, char *username, char *password){
  User userTable[TOTALUSERS];
  iNodo fileInode;
  int i, sessionIndex;

  if(loadUserTable(userTable, &fileInode, (Dir *) NULL))
    return 1;

  for(i=0; i<=TOTALUSERS; i++){
    if(i == TOTALUSERS)
      return 2;

    if(strcmp(username, userTable[i].name) == 0 && strcmp(password, userTable[i].password) == 0){
      // Si se llego a la cantidad maxima de sesiones
      // Reemplazamos la sesión mas antigua
      // Si no, solo tomamos una posicion vacia
      sessionIndex = getOldestSessionAccess(activeSessions);

      strcpy(activeSessions[sessionIndex].name, username);
      strcpy(activeSessions[sessionIndex].cwd, "/");
      activeSessions[sessionIndex].lastAccess = getCurrentTime();

      sesPtr = &activeSessions[sessionIndex];

      // Update lastAccess
      updateUserInfo(username, password, getCurrentTime());
      break;
    }
  }

  return 0;
}
