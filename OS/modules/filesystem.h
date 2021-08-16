#ifndef FS_H
#define FS_H

#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "globalMacros.h"
// #include "lexer.h"
#include "user.h"
#include "utils.h"
#include "fsDefinitions.h"


typedef struct freeQueue {
  int list[256];
  int start, end, capacity, size;
} FreeQueue;

void fsInit(void);
int fillLIL(iNodo *listaInodos);
int fillLBL(char *superBlock);
void initINodeList(iNodo *iNodeList);
void initSuperBlock(char *superBlock);
void initUserFile();
void crearRaiz(Dir *bloque, iNodo *listaInodos);
int load(char SB[2048], iNodo *LI);
void format(char superBlock[2048], iNodo *LI, Dir *root);
void list(char *outputBuffer, Dir directorio[64]);
int create(char *fileName, Dir parent, Session user, char isDir);
Dir namei(char *path, char *cwd);
int realPath(char *outputPath, char *path, char *cwd);
int getBlock(void *destination, int block);
int getInode(iNodo *destination, int inodo);
int writeInode(iNodo *newInode, int inodeNum);
int writeBlock(void *buffer, int block, size_t size);
void prepend(char* s, const char* t);
int delete(Dir file, Dir parent, Session user, char isDir);
int updateSuperblock(int block, char status);
int freeInode(int inode);
int freeBlock(int block);
int fileExistsInDir(char *fileName, int parentInodeNum);

int enqueue(FreeQueue *queue, int newVal);
int dequeue(FreeQueue *queue);

#endif
