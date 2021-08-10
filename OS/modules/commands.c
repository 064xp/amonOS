#include "commands.h"

/*
  Retvals
    0: Exitoso
    1: Directorio no encontrado
    2: Archivo no es un directorio
*/
int ls(char *outputBuffer, User *user, int argc, char argv[][TOKENLEN]){
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

int touch(char *outputBuffer, User *user, int argc, char argv[][TOKENLEN]){
  char fileName[12];
  char parentPath[TOKENLEN];
  Dir parent;

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

  if(create(fileName, parent, *user, 0)){
    strcpy(outputBuffer, "An error ocurred while creating the file");
    return 1;
  }

  sprintf(outputBuffer, "File \"%s\" created succesfullly", argv[1]);
  return 0;
}

int mkdir(char *outputBuffer, User *user, int argc, char argv[][TOKENLEN]){
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

  if(create(dirName, parent, *user, 1)){
    strcpy(outputBuffer, "An error ocurred while creating the directory");
    return 1;
  }

  sprintf(outputBuffer, "Directory \"%s\" created succesfullly", argv[1]);
  return 0;

}

int rm(char *outputBuffer, User *user, int argc, char argv[][TOKENLEN]){
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

int cd(char *outputBuffer, User *user, int argc, char argv[][TOKENLEN]){
  Dir newDir;

  if(argc < 2){
    strcpy(outputBuffer, "Missing arguments.\nUsage:\n\tcd [new directory]");
    return 1;
  }

  newDir = namei(argv[1], user->cwd);
  if(newDir.iNodo == -1){
    strcpy(outputBuffer, "No such file or directory");
    return 1;
  }

  if(argv[1][0] != '/'){
    prepend(argv[1], user->cwd);
  } else {
    strcpy(user->cwd, argv[1]);
  }
  return 0;
}
