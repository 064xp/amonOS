#include "filesystem.h"

FreeQueue LIL = {{}, 0, -1, 256, 0};
FreeQueue LBL = {{}, 0, -1, 256, 0};

void fsInit(){
  char super[2048];
  iNodo listaInodos[80]; // 5 bloques de 16 inodos
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

void fillLIL(iNodo listaInodos[16]){
  int i;

  for(i=0; i<80; i++){
    if(listaInodos[i].type == 0){
      // Enqueue número de I nodo
      enqueue(&LIL, i+1);
    }
    if(LIL.size == LIL.capacity)
      return;
  }
}

void fillLBL(char *superBlock){
  int i;

  for(i=0; i<2048; i++){
    if(superBlock[i] == 0){
      enqueue(&LBL, i+1);
    }
    if(LBL.size == LBL.capacity) return;
  }
}

void initINodeList(iNodo *iNodeList){
  int i;

  for(i=0; i<80; i++){
    iNodeList[i].type = 0;
  }

  iNodeList[0].type = 1; // reservado
}

/*
  Pone todos los valores en 0,
  0: Bloque libre
  1: Bloque ocupado
*/
void initSuperBlock(char *superBlock){
  int i;

  // Primeros 9 bloques ocupados
  for(i=0; i<9; i++){
    superBlock[i] = 1;
  }

  // El resto libres
  for(i=10; i<TOTALBLKS; i++){
    superBlock[i] = 0;
  }

  // Queda mas espacio en el super bloque, pero ya
  // no hay mas bloques disponibles
  for(i=TOTALBLKS; i<2048; i++){
    superBlock[i] = 1;
  }
}

void format(char superBlock[2048], iNodo *LI, Dir *root){
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

int load(char SB[2048], iNodo *LI){
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

void crearRaiz(Dir *bloque, iNodo *listaInodos){
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
  memcpy(&listaInodos[1], &inodoRaiz, sizeof(iNodo));
}

void list(char *outputBuffer, Dir directorio[64]){
  int i, inode;
  iNodo curFileInode;
  char tmpBuff[500];

  strcpy(outputBuffer, "");
  for(i=0; i<64; i++){
    inode = directorio[i].iNodo;
    getInode(&curFileInode, inode);
    if(inode != 0){
      sprintf(tmpBuff, " %i\t%c%c%c%c%c%c%c\t%i\t%s\t%i\t%i\t%s\n",
          inode,
          curFileInode.type,
          curFileInode.perms[0],
          curFileInode.perms[1],
          curFileInode.perms[2],
          curFileInode.perms[3],
          curFileInode.perms[4],
          curFileInode.perms[5],
          curFileInode.links,
          curFileInode.owner,
          curFileInode.size,
          curFileInode.lastModified,
          directorio[i].nombre
      );
    }
    strncat(outputBuffer, tmpBuff, strlen(tmpBuff));
    strcpy(tmpBuff, "");
  }
}

/*
  Obtiene un inodo de la LIL
    Si se vacía, intenta rellenarla otra vez
*/
int iget(){
  int inodeNum;
  iNodo listaInodos[80];
  // dequeue an inode from LIL
  if((inodeNum = dequeue(&LIL)) == -1){
    int fd;
    fd = open("Fs", O_RDONLY);
    if (fd==-1){
        perror("");
        return -1;
    }

    lseek(fd, 3072, SEEK_SET);
    read(fd, listaInodos, sizeof(iNodo)*5*16);
    close(fd);

    fillLIL(listaInodos);

    if(LIL.size == 0){
      return -1;
    }
  }

  return inodeNum;
}

int blkget(){
  int blkNum;
  char super[2048];
  // dequeue an inode from LBL
  if((blkNum = dequeue(&LBL)) == -1){
    int fd;
    fd = open("Fs", O_RDONLY);
    if (fd==-1){
        perror("");
        return -1;
    }

    lseek(fd, 1024, SEEK_SET);
    read(fd, super, 2048);
    close(fd);

    fillLBL((char *)&super);

    if(LBL.size == 0){
      return -1;
    }
  }

  return blkNum;
}

/*
  Crear archivo o directorio

  TODO:
    - Validar si ya no hay mas espacio en el bloque del
      directorio padre y conseguir otro bloque
*/
int create(char *fileName, Dir parent, User user, char isDir){
  int i, j, inode, block;
  Dir tmpDir[64]; //1024 bytes
  iNodo parentInode, newInode;
  Dir parentDir[64];

  //TODO: check perms

  if(parent.iNodo == -1){
    return 1;
  }

  inode = iget();
  if(inode == -1)
    return 1;

  block = blkget();
  if(block == -1)
    return 1;

  printf("new i node %i\nnew block %i\n", inode, block);

  getInode(&parentInode, parent.iNodo);
  getBlock(&parentDir, parentInode.contentTable[0]);

  strcpy(tmpDir[0].nombre, ".");
  tmpDir[0].iNodo = inode;
  strcpy(tmpDir[1].nombre, "..");
  tmpDir[1].iNodo = parent.iNodo;

  if(isDir){
    for(i=2; i<64; i++){
      tmpDir[i].iNodo = 0;
    }
  }

  for(i=0; i<64; i++){
    if(parentDir[i].iNodo == 0){
      for(j=0; j<9; j++){
        newInode.contentTable[j] = 0;
      }

      if(isDir){
        newInode.type = 'd';
        strcpy(newInode.perms, "rwxr--");
        newInode.size = 1024;
      } else {
        newInode.type = '-';
        strcpy(newInode.perms, "rw-r--");
        newInode.size = 0;
      }

      newInode.links = 1;
      strcpy(newInode.owner, user.name);
      newInode.lastModified = getCurrentTime();
      newInode.contentTable[0] = block;

      // Escribir bloque e I Nodo del nuevo archivo/directorio
      if(isDir){
        writeBlock(tmpDir, block, sizeof(Dir)*64);
      }
      writeInode(&newInode, inode);
      // Actualizar superbloque para marcar al bloque como ocupado
      updateSuperblock(block, 1);

      // Registrar el nuevo archivo en el directorio padre
      parentDir[i].iNodo = inode;
      strcpy(parentDir[i].nombre, fileName);
      writeBlock(parentDir, parentInode.contentTable[0], sizeof(Dir)*64);

      break;
    } //fin if
  } //fin for
  return 0;
}

int delete(char datos[][1024],iNodo listaInodos[][16], int *indiceLBL, int *indiceLIL, int *LIL, int *LBL){
  // char buffer[200];
  // iNodo *fileInode;
  // Dir *file;
  // int i;
  //
  // printf("Ingresa el nombre del archivo:\n");
  // scanf("%s", buffer );
  //
  // if(strcmp(buffer, "/") == 0){
  //   printf("No se puede borrar el directorio raiz\n");
  //   return -1;
  // }
  //
  // if(buffer[0] != '/')
  //   prepend(buffer, "/");
  //
  // namei(datos, listaInodos, buffer);
  // if(file == NULL){
  //   printf("No se econtro el archivo\n");
  //   return -1;
  // }
  //
  // fileInode = getInode(listaInodos, file->iNodo);
  //
  // // Liberar bloques
  // for(i=0; i<9; i++){
  //   if(fileInode->contentTable[i] != 0){
  //     (*indiceLBL)--;
  //     LBL[*indiceLBL] = fileInode->contentTable[i];
  //   } else {
  //     break;
  //   }
  // }
  //
  // // Liberar el I Nodo
  // (*indiceLIL)--;
  // LIL[*indiceLIL] = file->iNodo;
  // file->iNodo = 0;
  //
  return 0;
}

int getInode(iNodo *destination, int inode){
  int fd;
  fd = open("Fs", O_RDONLY);
  if (fd==-1){
      perror("");
      return -1;
  }

  lseek(fd, (3072) + ((inode-1) * sizeof(iNodo)), SEEK_SET);
  read(fd, destination, sizeof(iNodo));
  close(fd);

  return 0;
}

int writeInode(iNodo *newInode, int inodeNum){
  int fd;
  fd = open("Fs", O_WRONLY);
  if (fd==-1){
      perror("");
      return -1;
  }

  lseek(fd, (3072) + ((inodeNum-1) * sizeof(iNodo)), SEEK_SET);
  write(fd, newInode, sizeof(iNodo));
  close(fd);

  return 0;
}

int getBlock(void *destination, int block){
  int fd;
  fd = open("Fs", O_RDONLY);
  if (fd==-1){
      perror("");
      return -1;
  }

  lseek(fd, 1024 * (block -1), SEEK_SET);
  read(fd, destination, 1024);
  close(fd);

  return 0;
}

int writeBlock(void *buffer, int block, size_t size){
  int fd;
  fd = open("Fs", O_WRONLY);
  if (fd==-1){
      perror("");
      return -1;
  }

  lseek(fd, 1024 * (block -1), SEEK_SET);
  write(fd, buffer, size);
  close(fd);

  return 0;
}

int updateSuperblock(int block, char status){
  int fd;
  fd = open("Fs", O_WRONLY);
  if (fd==-1){
      perror("");
      return -1;
  }

  lseek(fd, 1024 + block-1, SEEK_SET);
  write(fd, &status, 1);
  close(fd);

  return 0;

}

Dir namei(char *path, char *cwd){
  Dir file = {-1, ""};
  Dir curDir[64];
  iNodo curInode;
  char buffer[100], search = 0;
  int i, j, cnt = 0;

  //comenzamos en dir raiz
  if(getBlock(curDir, 9) == -1){
    printf("Error getBlock\n");
    return file;
  }

  if(strcmp(path, "/") == 0){
    return curDir[0];
  }

  // Si no es una ruta absoluta, agregar el CWD antes
  if(path[0] != '/'){
    prepend(path, cwd);
  }

  for(i=1; i<strlen(path); i++){
    // Separamos en los espacios, en las diagonales, o
    // si es final de cadena

    // Ponemos el token leido en buffer y si search = 1
    // Busca el nombre que se tiene en el buffer en el directorio
    // actual
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

    if(search){
      //buscar archivo con el nombre actual en el directorio
      cnt = 0;
      search = 0;
      for(j=0; j<64; j++){
        if(curDir[j].nombre[0] == '\0'){
          break;
        } else if(strcmp(curDir[j].nombre, buffer) == 0){
          file = curDir[j];
          getInode(&curInode, file.iNodo);
          getBlock(curDir, curInode.contentTable[0]);
          break;
        }
      }
    }

    cnt++;
  }

  // Si no se encontró el archivo
  if(strcmp(file.nombre, buffer) != 0){
    file.iNodo = -1;
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
