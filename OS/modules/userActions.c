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
  Dir file;
  if(userFile == NULL){
    userFile = &file;
  }
  *userFile = namei("/etc/user", NULL);
  if(userFile->iNodo == -1){
    return 1;
  }
  getInode(fileInode, userFile->iNodo);
  getBlock(userTable, fileInode->contentTable[0]);

  return 0;
}

/*
Obtiene el indice de la sesión, si existe una sesión de dicho usuario
  Retvals
    -1: No se econtró una sesión de ese usuario
    X: Cualquier otro número es el índice en la lista de sesiones
*/
int getOpenSesssionIndex(char *username){
  int i;

  for(i=0; i<=MAX_SESSIONS; i++){
    if(i == MAX_SESSIONS)
      return -1;

    if(strcmp(activeSessions[i].name, username) == 0)
      return i;
  }

  return 0;
}

/*
  Retvals
    0: Exitoso
    1: Error al leer la tabla de usuarios
    2: Usuario no encontrado / Credenciales incorrectas
*/
int createSession(Session *sesPtr, char *username, char *password){
  User userTable[TOTALUSERS];
  iNodo fileInode;
  int i, sessionIndex;

  if(loadUserTable(userTable, &fileInode, (Dir *) NULL))
    return 1;

  for(i=0; i<=TOTALUSERS; i++){
    if(i == TOTALUSERS)
      return 2;

    if(strcmp(username, userTable[i].name) == 0 && strcmp(password, userTable[i].password) == 0){
      // Buscamos si ese usuario ya tiene una sesion activa
      if((sessionIndex = getOpenSesssionIndex(username)) == -1){
        // Si no tiene sesion, creamos una en el primer lugar libre de la lista
        // Si la lista esta llena, reemplazamos la sesion mas antigua de la lista
        sessionIndex = getOldestSessionAccess(activeSessions);
      }

      strcpy(activeSessions[sessionIndex].name, username);
      strcpy(activeSessions[sessionIndex].cwd, "/");
      activeSessions[sessionIndex].lastAccess = getCurrentTime();

      sesPtr = &activeSessions[sessionIndex];

      //Update lastAccess
      updateUserInfo(username, password, getCurrentTime());
      break;
    }
  }

  return 0;
}

void initSessions(){
  int i;
  for(i=0; i<MAX_SESSIONS; i++){
    strcpy(activeSessions[i].name, "");
    strcpy(activeSessions[i].cwd, "");
    activeSessions[i].lastAccess = 0;
  }
}


void executeAuthenticated(RequestPacket *request, ResponsePacket *response){
  Session *userSession;
  Session nullSession = {
    "",
    "/",
    0
  };
  int sessionIndex;

  if((sessionIndex = getOpenSesssionIndex(request->user)) == -1){
    userSession = &nullSession;
  } else {
    userSession = &activeSessions[sessionIndex];
  }

  executeCommand(response->buffer, userSession, request->buffer);

  strcpy(response->user, userSession->name);
  strcpy(response->cwd, userSession->cwd);
}
