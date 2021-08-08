#include "commands.h"

/*
  Retvals
    0: Exitoso
    1: Directorio no encontrado
    2: Archivo no es un directorio
    3: Faltan parámetros
*/
int ls(char *outputBuffer, int argc, char argv[][TOKENLEN]){
  Dir foundFile;
  iNodo dirInode;
  Dir directory[64];

  if(argc < 2){
    strcpy(outputBuffer, "Too few parameters.\nUsage:\n  ls [path]");
    return 3;
  }

  foundFile = namei(argv[1]);
  if(foundFile.iNodo == -1){
    strcpy(outputBuffer, "Directory not found");
    return 1;
  }

  getInode(&dirInode, foundFile.iNodo);
  if(dirInode.type != 'd'){
    strcpy(outputBuffer, argv[1]);
    strncat(outputBuffer, " is not a directory", 20);
    return 2;
  }

  getBlock(directory, dirInode.contentTable[0]);

  list(outputBuffer, directory);

  return 0;
}
