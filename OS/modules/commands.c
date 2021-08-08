#include "commands.h"

/*
  Retvals
    0: Exitoso
    1: Directorio no encontrado
    2: Archivo no es un directorio
*/
int ls(char *outputBuffer, int argc, char argv[][TOKENLEN]){
  Dir foundFile;
  iNodo dirInode;
  Dir directory[64];

  foundFile = namei(argv[1]);
  if(foundFile.iNodo == -1){
    return 1;
  }


  getInode(&dirInode, foundFile.iNodo);
  if(dirInode.type != 'd'){
    return 2;
  }

  getBlock(directory, dirInode.contentTable[0]);

  list(outputBuffer, directory);

  return 0;
}
