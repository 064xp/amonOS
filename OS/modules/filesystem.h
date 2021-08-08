#ifndef FS_H
#define FS_H

#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "globalMacros.h"
#include "user.h"

//64 bytes total
typedef struct iNodo {
  char type;
  char perms[6];
  unsigned char links;
  char owner[12];
  unsigned int size;
  unsigned int lastModified;
  int contentTable[9];
} iNodo;

typedef struct dir {
  int iNodo;
  char nombre[12];
} Dir;

typedef struct freeQueue {
  int list[256];
  int start, end, capacity, size;
} FreeQueue;

void fsInit(void);
void fillLIL(iNodo listaInodos[][16]);
void fillLBL(char *superBlock);
void initINodeList(iNodo iNodeList[][16]);
void initSuperBlock(char *superBlock);
void crearRaiz(Dir *bloque, iNodo listaInodos[][16]);
int load(char SB[2048], iNodo LI[][16]);
void format(char superBlock[2048], iNodo LI[][16], Dir *root);
void list(char *outputBuffer, Dir directorio[64]);
void create(char *fileName, User user, char isDir);
Dir namei(char *path, User user);
int getBlock(void *destination, int block);
int getInode(iNodo *destination, int inodo);
void prepend(char* s, const char* t);
int delete(char datos[][1024],iNodo listaInodos[][16], int *indiceLBL, int *indiceLIL, int *LIL, int *LBL);

int enqueue(FreeQueue *queue, int newVal);
int dequeue(FreeQueue *queue);
time_t getCurrentTime();
void prepend(char* s, const char* t);

#endif
