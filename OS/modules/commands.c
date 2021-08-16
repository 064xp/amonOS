#include "commands.h"

char lsDescription[] = "List the files in the given directory.";
char touchDescription[] = "Create an empty text file.";
char clearDescription[] = "Clear the terminal screen.";
char mkdirDescription[] = "Create a new directory.";
char rmDescription[] = "Delete a file.";
char cdDescription[] = "Move into a new working directory.";
char pwdDescription[] = "Print current working directory absolute path.";
char catDescription[] = "Display the contents of one or more files.";
char createuserDescription[] = "Create a new user.";
char helpDescription[] = "Display a list of commands.";
char writefDescription[] = "Write text into a file.";
char loginDescription[] = "Login as a certain user.";
char logoutDescription[] = "Logout as current user.";
char rrDescription[] = "See round robin scheduling log.";

// Lista de comandos
Command commands [COMMANDSLEN] = {
  {"ls", &ls, 1, lsDescription},
  {"touch", &touch, 1, touchDescription},
  {"mkdir", &mkdir, 1, mkdirDescription},
  {"rm", &rm, 1, rmDescription},
  {"cd", &cd, 1, cdDescription},
  {"pwd", &pwd, 1, pwdDescription},
  {"writef", &writef, 1, writefDescription},
  {"cat", &cat, 1, catDescription},
  {"createuser", &createUser, 0, createuserDescription},
  {"login", &login, 0, loginDescription},
  {"logout", &logout, 1, logoutDescription},
  {"rr", &rr, 1, rrDescription},
  {"clear", (void *)NULL, 0, clearDescription},
  {"help", &help, 0, helpDescription}
};

/*
  Retvals
    0: Exitoso
    1: Directorio no encontrado
    2: Archivo no es un directorio
*/
int ls(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]){
  Dir foundFile;
  iNodo dirInode;
  Dir directory[64];


  // Si se hace ls sin argumentos, listar cwd
  if(argc < 2){
    foundFile = namei(user->cwd, "");
  } else {
    foundFile = namei(argv[1], user->cwd);
  }

  if(foundFile.iNodo == -1){
    strcpy(outputBuffer, "Directory not found");
    return 1;
  }

  getInode(&dirInode, foundFile.iNodo);
  if(dirInode.type != 'd'){
    sprintf(outputBuffer, "%s is not a directory", argv[1]);
    return 2;
  }

  getBlock(directory, dirInode.contentTable[0]);

  list(outputBuffer, directory);

  return 0;
}

int touch(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]){
  char fileName[12];
  char parentPath[TOKENLEN];
  Dir parent;
  int err;

  if(argc < 2){
    strcpy(outputBuffer, "Missing arguments.\nUsage:\n\ttouch [fileName]");
    return 1;
  }

  separeParentPath(argv[1], parentPath, fileName);
  if(strlen(parentPath) == 0)
    strcpy(parentPath, user->cwd);

  parent = namei(parentPath, user->cwd);
  if(parent.iNodo == -1){
    strcpy(outputBuffer, "Invalid file location");
    return 1;
  }

  err = create(fileName, parent, *user, 0);
  if(err == 1){
    strcpy(outputBuffer, "An error ocurred while creating the file");
    return 1;
  } else if(err == 2){
    sprintf(outputBuffer, "File \"%s\" already exists", argv[1]);
    return 1;
  }

  sprintf(outputBuffer, "File \"%s\" created succesfullly", argv[1]);
  return 0;
}

int mkdir(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]){
  char dirName[12];
  char parentPath[TOKENLEN];
  Dir parent;

  if(argc < 2){
    strcpy(outputBuffer, "Missing arguments.\nUsage:\n\tmkdir [fileName]");
    return 1;
  }

  separeParentPath(argv[1], parentPath, dirName);
  if(strlen(parentPath) == 0)
    strcpy(parentPath, user->cwd);

  parent = namei(parentPath, user->cwd);
  if(parent.iNodo == -1){
    strcpy(outputBuffer, "Invalid file location");
    return 1;
  }

  if(create(dirName, parent, *user, 1) <= 2){
    strcpy(outputBuffer, "An error ocurred while creating the directory");
    return 1;
  }

  sprintf(outputBuffer, "Directory \"%s\" created succesfullly", argv[1]);
  return 0;

}

int rm(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]){
  char parentPath[TOKENLEN];
  Dir file, parent;
  iNodo fileInode;

  if(argc < 2){
    strcpy(outputBuffer, "Missing arguments.\nUsage:\n\trm [fileName]");
    return 1;
  }

  file = namei(argv[1], user->cwd);
  if(file.iNodo == -1){
    sprintf(outputBuffer, "File \"%s\" not found", argv[1]);
    return 1;
  }
  getInode(&fileInode, file.iNodo);

  // Aun no esta implementado el eliminado recursivo de directorios
  if(fileInode.type == 'd'){
    strcpy(outputBuffer, "Deletion of directories is not yet implemented");
    return 1;
  }

  separeParentPath(argv[1], parentPath, NULL);
  if(strlen(parentPath) == 0)
    strcpy(parentPath, user->cwd);

  parent = namei(parentPath, user->cwd);

  if(parent.iNodo == -1){
    strcpy(outputBuffer, "Invalid file location");
    return 1;
  }

  if(delete(file, parent, *user, 0)){
    strcpy(outputBuffer, "An error ocurred while deleting the file");
    return 1;
  }

  sprintf(outputBuffer, "File \"%s\" deleted succesfullly", argv[1]);
  return 0;
}

int cd(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]){
  char path[TOKENLEN];

  if(argc < 2){
    strcpy(outputBuffer, "Missing arguments.\nUsage:\n\tcd [new directory]");
    return 1;
  }

  if(realPath(path, argv[1], user->cwd)){
    strcpy(outputBuffer, "No such file or directory");
    return 1;
  }

  strcpy(user->cwd, path);
  strcpy(outputBuffer, "");
  return 0;
}

int pwd(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]){
  strcpy(outputBuffer, user->cwd);
  return 0;
}

int writef(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]){
  char fileContents[1024] = "", newFileBuff[1024] = "", filePath[TOKENLEN];
  int i, bytesToWrite, newLen, currentLen;
  iNodo fileInode;
  Dir file;

  if(argc < 3){
    strcpy(outputBuffer, "Missing arguments.\nUsage:\n\twritef [options] [file] [file contents]\n\tOptions:\n\t\t-a: Append text to the end of the file");
    return 1;
  }

  if(argv[1][0] == '-'){
    i = 3;
    strcpy(filePath, argv[2]);
  }
  else{
    i = 2;
    strcpy(filePath, argv[1]);
  }

  file = namei(filePath, user->cwd);

  if(file.iNodo == -1){
    sprintf(outputBuffer, "File \"%s\" not found", filePath);
    return 1;
  }

  getInode(&fileInode, file.iNodo);

  if(strcmp(argv[1], "-a") == 0){
    getBlock(fileContents, fileInode.contentTable[0]);
  }

  for(; i<argc; i++){
    strncat(newFileBuff, argv[i], strlen(argv[i]));
    strncat(newFileBuff, " ", 2);
  }

  currentLen = strlen(fileContents);
  newLen = strlen(newFileBuff);

  // En caso de que el tamaño sobrepase 1k
  if(newLen + currentLen >= 1024){
    printf("Overflow condition\n");
    bytesToWrite = newLen - ((currentLen + newLen) - 1023);
  } else {
    bytesToWrite = newLen;
  }

  strncat(fileContents, newFileBuff, bytesToWrite);

  // actualizar tamaño
  fileInode.size = strlen(fileContents);
  writeInode(&fileInode, file.iNodo);

  // Escribir a disco
  writeBlock(fileContents, fileInode.contentTable[0], 1024);

  sprintf(outputBuffer, "%i bytes written to file \"%s\"", bytesToWrite, filePath);

  return 0;
}

int cat(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]){
  char tempBuffer[1024];
  iNodo fileInode;
  char *buffer, *bu;
  Dir file;
  int i, bytesToWrite, bufferLen;

  if(argc < 2){
    strcpy(outputBuffer, "Missing arguments.\nUsage:\n\tcat [file1]...[file N]");
    return 1;
  }

  bufferLen = 1024 * argc-1;
  buffer = malloc(bufferLen);
  bu = buffer;
  memset(buffer, 0, bufferLen);
  memset(outputBuffer, 0, OUTPUT_BUF_LEN);

  for(i=1; i<argc; i++){
    file = namei(argv[i], user->cwd);
    if(file.iNodo == -1){
      sprintf(outputBuffer, "File \"%s\" not found", argv[i]);
      free(bu);
      break;
      return 1;
    }
    getInode(&fileInode, file.iNodo);
    getBlock(tempBuffer, fileInode.contentTable[0]);
    strcat(buffer, tempBuffer);
  }

  bufferLen = strlen(buffer);
  bytesToWrite = bufferLen > OUTPUT_BUF_LEN ? OUTPUT_BUF_LEN : bufferLen;

  memcpy(outputBuffer, buffer, bytesToWrite);
  outputBuffer[OUTPUT_BUF_LEN -1] = '\0';
  free(bu);
  return 0;
}

int createUser(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]){
  int err;

  if(argc < 3){
    strcpy(outputBuffer, "Missing arguments.\nUsage:\n\tcreateuser [username] [password]");
    return 1;
  }

  err = addUser(argv[1], argv[2]);
  if(err == 1){
    strcpy(outputBuffer, "An error ocurred while creating a new user");
    return 1;
  } else if(err == 2){
    strcpy(outputBuffer, "Error: Maximum ammount of users reached");
    return 1;
  }

  sprintf(outputBuffer, "User \"%s\" created succesfullly", argv[1]);
  return 0;
}

int login(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]){
  int retVal;

  if(argc < 3){
    strcpy(outputBuffer, "Missing arguments.\nUsage:\n\tlogin [username] [password]");
    return 1;
  }

  retVal = createSession(user, argv[1], argv[2]);

  if(retVal == 1){
    strcpy(outputBuffer, "An error ocurred while reading the user table.");
    return 1;
  } else if(retVal == 2){
    strcpy(outputBuffer, "Incorrect credentials");
    return 1;
  }

  sprintf(outputBuffer, "Logged in succesfullly as user \"%s\"", argv[1]);
  return 0;
}

int logout(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]){
  char username[12];
  strcpy(username, user->name);
  deleteSession(user);
  sprintf(outputBuffer, "Logged out of user \"%s\"", username);

  return 0;
}

int rr(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]){
  int fd, offset, bytesToRead;

  fd = open("rr.log", O_RDONLY);
  if(fd == -1){
    strcpy(outputBuffer, "Scheduling log is empty");
    return 1;
  }

  offset = lseek(fd, 0, SEEK_END);
  if(offset < OUTPUT_BUF_LEN){
    bytesToRead = offset;
    offset = 0;
  } else {
    offset = offset - OUTPUT_BUF_LEN;
    bytesToRead = OUTPUT_BUF_LEN;
  }

  lseek(fd, offset, SEEK_SET);
  read(fd, outputBuffer, bytesToRead);
  return 0;
}

int help(char *outputBuffer, Session *user, int argc, char argv[][TOKENLEN]){
  int i, j, separation = 16, spaces;

  strcpy(outputBuffer, "");
  strcat(outputBuffer, "Command\t\tDescription\n\n");
  for(i=0; i<COUNT_OF(commands); i++){
    spaces = separation - strlen(commands[i].name);

    strcat(outputBuffer, commands[i].name);
    for(j=0; j<spaces; j++){
      strcat(outputBuffer, " ");
    }
    strcat(outputBuffer, commands[i].description);
    strcat(outputBuffer, "\n");
  }

  return 0;
}
