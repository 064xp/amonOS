#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define TOTALBLKS 58

void format(char bb[1024], char SB[2048], iNodo LI[5][16], char datos[50][1024], int *indiceLIL, int *indiceLBL);
void list(Dir directorio[64], iNodo listaInodos[5][16]);
void create(char datos[][1024],iNodo listaInodos[][16], int *indiceLBL, int *indiceLIL, int *LIL, int *LBL,char isDir);
Dir *namei(char datos[][1024], iNodo listaInodos[][16], char *path);
void *getBlock(char datos[][1024], iNodo *inode, int blockIdx);
iNodo *getInode(iNodo listaInodos[][16], int inodo);
void prepend(char* s, const char* t);
void crearRaiz(char *bloque, iNodo listaInodos[][16]);
int load(char bb[1024], char SB[2048], iNodo LI[5][16], char datos[50][1024], int *indiceLIL, int *indiceLBL);
int delete(char datos[][1024],iNodo listaInodos[][16], int *indiceLBL, int *indiceLIL, int *LIL, int *LBL);

#endif
