#include "commands.h"

/*
  Retvals
    0: Exitoso
    1: Directorio no encontrado
    2: Archivo no es un directorio
*/
int ls(char *outputBuffer, User user, int argc, char argv[][TOKENLEN]){
  Dir foundFile;
  iNodo dirInode;
  Dir directory[64];


  // Si se hace ls sin argumentos, listar cwd
  if(argc < 2){
    foundFile = namei(user.cwd, user);
  } else {
    foundFile = namei(argv[1], user);
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
