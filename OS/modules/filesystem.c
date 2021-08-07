#include "filesystem.h"

FreeQueue LIL = {{}, 0, -1, 256, 0};
FreeQueue LBL = {{}, 0, -1, 256, 0};

void fsInit(){
  char super[2048];
  iNodo listaInodos[5][16];
  Dir root[64];

  if(load(super, listaInodos) == -1){
    printf("File System no encontrado, creando raiz...\n\n");
    initINodeList(listaInodos);
    initSuperBlock(super);
    crearRaiz(root, listaInodos);
    format(super, listaInodos, root);
  } else {
    printf("File System encontrado, cargando...\n\n");
  }

  fillLIL(listaInodos);
  fillLBL(super);
}

void fillLIL(iNodo listaInodos[][16]){
  int i, j;

  for(i=0; i<5; i++){
    for(j=0; j<16; j++){
      if(listaInodos[i][j].type == 0){
        // Enqueue número de I nodo
        enqueue(&LIL, (16*i) + (j+1));
      }
      if(LIL.size == LIL.capacity)
        return;
    }
  }
}

void fillLBL(char *superBlock){
  int i;

  for(i=0; i<2048; i++){
    if(superBlock[i] == 0){
      enqueue(&LBL, i);
    }
    if(LBL.size == LBL.capacity) return;
  }
}

void initINodeList(iNodo iNodeList[][16]){
  int i, j;

  for(i=0; i<5; i++){
    for(j=0; j<16; j++){
      iNodeList[i][j].type = 0;
    }
  }

  iNodeList[0][0].type = '-';
}

/*
  Pone todos los valores en 0,
  0: Bloque libre
  1: Bloque ocupado
*/
void initSuperBlock(char *superBlock){
  int i;

  for(i=10; i<TOTALBLKS; i++){
    superBlock[i] = 0;
  }

  for(i=TOTALBLKS; i<2048; i++){
    superBlock[i] = 1;
  }
}

void format(char superBlock[2048], iNodo LI[][16], Dir *root){
    //bloques
    // 1024 - 8 = 1016 bloques restantes
    // 4 + 12 = 16 bytes/archivo (struct dir)
    //1 bloque = 1024 bytes
    // 1024 / 16 bytes(archivo) = 64 archivos/bloque
    char bootBlock[1024] = {0};
    char datos[TOTALBLKS - 8][1024];
    int fd;

    fd = open("Fs", O_WRONLY|O_CREAT);
    if (fd==-1){
        perror("");
        return;
    }

    memcpy(datos[0], root, sizeof(Dir) * 64);
    write(fd, bootBlock, 1024);
    write(fd, superBlock, 2048);
    write(fd, LI, sizeof(iNodo)*5*16);
    write(fd, datos, 50 * 1024);
    close(fd);
}

int load(char SB[2048], iNodo LI[][16]){
  int fd;
  fd = open("Fs", O_RDONLY);
  if (fd==-1){
      perror("");
      return -1;
  }

  lseek(fd, 1024, SEEK_SET);
  read(fd, SB, 2048);
  read(fd, LI, sizeof(iNodo)*5*16);
  close(fd);

  return 0;
}

void crearRaiz(Dir *bloque, iNodo listaInodos[][16]){
  int i;
  Dir root[64] = {
    {2, "."}, //Numero de I Nodo, nombre dir
    {2, ".."},
  };
  iNodo inodoRaiz= {
    'd', // Tipo
    "rwxrwx", //Perms
    1, //links
    "root", //Owner
    1024, //size 1kB
    getCurrentTime(), //Last modified
    {9} // Content table
  };

  for(i=2; i<64; i++){
    root[i].iNodo = 0;
  }
  memcpy(bloque, root, sizeof(Dir) * 64);
  memcpy(&listaInodos[0][1], &inodoRaiz, sizeof(iNodo));
}

void list(Dir directorio[64], iNodo listaInodos[5][16]){
  int i, iNodo;

  for(i=0; i<64; i++){
    iNodo = directorio[i].iNodo;
    if(iNodo != 0){
      printf(" %i\t%c%c%c%c%c%c%c\t%i\t%s\t%i\t%i\t%s\n",
          iNodo,
          listaInodos[iNodo/16][(iNodo%16)-1].type,
          listaInodos[iNodo/16][(iNodo%16)-1].perms[0],
          listaInodos[iNodo/16][(iNodo%16)-1].perms[1],
          listaInodos[iNodo/16][(iNodo%16)-1].perms[2],
          listaInodos[iNodo/16][(iNodo%16)-1].perms[3],
          listaInodos[iNodo/16][(iNodo%16)-1].perms[4],
          listaInodos[iNodo/16][(iNodo%16)-1].perms[5],
          listaInodos[iNodo/16][(iNodo%16)-1].links,
          listaInodos[iNodo/16][(iNodo%16)-1].owner,
          listaInodos[iNodo/16][(iNodo%16)-1].size,
          listaInodos[iNodo/16][(iNodo%16)-1].lastModified,
          directorio[i].nombre
      );
    }
  }
}

void create(
  char datos[][1024],
  iNodo listaInodos[][16],
  int *indiceLBL,
  int *indiceLIL,
  int *LIL,
  int *LBL,
  char isDir
){
  char nombre[12], contenido[1008];
  int i, j;
  Dir tmpDir[64]; //1024 bytes
  Dir *root = (Dir *) datos[0];

  if(isDir){
    printf("Nombre del Directorio:\n");
    scanf("%s", nombre);

    strcpy(tmpDir[0].nombre, ".");
    tmpDir[0].iNodo = LIL[*indiceLIL];
    strcpy(tmpDir[1].nombre, "..");
    tmpDir[1].iNodo = 2;

    for(i=2; i<64; i++){
      tmpDir[i].iNodo = 0;
    }
  } else {
    printf("Nombre del Archivo:\n");
    scanf("%s", nombre);
    printf("Contenido del Archivo:\n");
    scanf("%s", contenido);
  }


  for(i=0; i<64; i++){
    if(root[i].iNodo == 0){
      for(j=0; j<9; j++){
        listaInodos[LIL[*indiceLIL]/16][(LIL[*indiceLIL] % 16)-1].contentTable[j] = 0;
      }
      if(isDir){
        listaInodos[LIL[*indiceLIL]/16][(LIL[*indiceLIL] % 16)-1].type = 'd';
        strcpy(listaInodos[LIL[*indiceLIL]/16][(LIL[*indiceLIL] % 16)-1].perms, "rwxr--");
        listaInodos[LIL[*indiceLIL]/16][(LIL[*indiceLIL] % 16)-1].size = 1024;
      } else {
        listaInodos[LIL[*indiceLIL]/16][(LIL[*indiceLIL] % 16)-1].type = '-';
        strcpy(listaInodos[LIL[*indiceLIL]/16][(LIL[*indiceLIL] % 16)-1].perms, "rw-r--");
        listaInodos[LIL[*indiceLIL]/16][(LIL[*indiceLIL] % 16)-1].size = strlen(contenido);
      }

      listaInodos[LIL[*indiceLIL]/16][(LIL[*indiceLIL] % 16)-1].links = 1;
      strcpy(listaInodos[LIL[*indiceLIL]/16][(LIL[*indiceLIL] % 16)-1].owner, "paulo");
      listaInodos[LIL[*indiceLIL]/16][(LIL[*indiceLIL] % 16)-1].lastModified = 2222;
      listaInodos[LIL[*indiceLIL]/16][(LIL[*indiceLIL] % 16)-1].contentTable[0] = LBL[*indiceLBL];

      // -9 ya que hay 9 bloques ocupados inicialmente
      // datos comienza desde el bloque 8 + 1 bloque del directorio raiz
      if(isDir){
        memcpy(datos[LBL[*indiceLBL]-9], tmpDir, sizeof(Dir) * 64);
      } else {
        strcpy(datos[LBL[*indiceLBL]-9], contenido);
      }

      root[i].iNodo = LIL[*indiceLIL];
      strcpy(root[i].nombre, nombre);

      (*indiceLIL)++;
      (*indiceLBL)++;

      break;
    } //fin if
  } //fin for
}

int delete(char datos[][1024],iNodo listaInodos[][16], int *indiceLBL, int *indiceLIL, int *LIL, int *LBL){
  char buffer[200];
  iNodo *fileInode;
  Dir *file;
  int i;

  printf("Ingresa el nombre del archivo:\n");
  scanf("%s", buffer );

  if(strcmp(buffer, "/") == 0){
    printf("No se puede borrar el directorio raiz\n");
    return -1;
  }

  if(buffer[0] != '/')
    prepend(buffer, "/");

  file = namei(datos, listaInodos, buffer);
  if(file == NULL){
    printf("No se econtro el archivo\n");
    return -1;
  }

  fileInode = getInode(listaInodos, file->iNodo);

  // Liberar bloques
  for(i=0; i<9; i++){
    if(fileInode->contentTable[i] != 0){
      (*indiceLBL)--;
      LBL[*indiceLBL] = fileInode->contentTable[i];
    } else {
      break;
    }
  }

  // Liberar el I Nodo
  (*indiceLIL)--;
  LIL[*indiceLIL] = file->iNodo;
  file->iNodo = 0;

  return 0;
}


iNodo *getInode(iNodo listaInodos[][16], int inodo){
  iNodo *in = &(listaInodos[inodo/16][(inodo % 16)-1]);
  return in;
}

void *getBlock(char datos[][1024], iNodo *inode, int blockIdx){
  int blockNum = inode->contentTable[blockIdx];
  return datos[blockNum -9];
}

Dir *namei(char datos[][1024], iNodo listaInodos[][16], char *path){
  Dir *file = NULL;
  Dir *curDir = (Dir *) datos[0]; //comenzamos en dir raiz
  iNodo *curInode;
  char buffer[100], search = 0;
  int i, j, cnt = 0;

  for(i=1; i<strlen(path)+1; i++){
    if(search){
      //buscar archivo con el nombre actual en el directorio
      search = 0;
      for(j=0; j<64; j++){
        if(curDir[j].nombre[0] == '\0'){
          break;
        } else if(strcmp(curDir[j].nombre, buffer) == 0){
          file = &curDir[j];
          curInode = getInode(listaInodos, file->iNodo);
          curDir = (Dir *) getBlock(datos, curInode, 0);
          break;
        }
      }
    }

    if(path[i] == '/' || path[i] == ' ' ){
      buffer[cnt] = '\0';
      search = 1;
    } else if(path[i+1] == '\0'){
      buffer[cnt] = path[i];
      buffer[cnt+1] = '\0';
      search = 1;
    } else {
      buffer[cnt] = path[i];
    }
    cnt++;
  }
  return file;
}


// Funciones de Fila

int enqueue(FreeQueue *queue, int newVal){
    if(queue->size == queue->capacity){
        return 1;
    }
    //Lo tratamos como un arreglo circular, para evitar recorrer a todos los elementos al quitar uno
    //https://en.wikipedia.org/wiki/Circular_buffer#Circular_buffer_mechanics
    //indice = (indice + 1) % tamaño máximo
    queue->end = (queue->end + 1) % queue->capacity;
    queue->list[queue->end] = newVal;
    queue->size++;
    return 0;
}

int dequeue(FreeQueue *queue){
    int element = -1;

    if(queue->size == 0){
        return element;
    }
    element = queue->list[queue->start];
    queue->start = (queue->start+1) % queue->capacity;
    queue->size--;
    return element;
}


time_t getCurrentTime(){
  time_t  secs;
  struct timespec spec;

  clock_gettime(CLOCK_REALTIME, &spec);
  secs  = spec.tv_sec;

  return secs;
}


void prepend(char* s, const char* t){
    size_t len = strlen(t);
    memmove(s + len, s, strlen(s) + 1);
    memcpy(s, t, len);
}
