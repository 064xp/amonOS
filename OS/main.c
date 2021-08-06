/****
Equipo:
  Paulo Eduardo Zamora Vanegas
  Gerardo Castruita Lopez
****/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

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

typedef struct PCB {
  char estado;
  int pid;
  char progCounter;
  int tEjec;
} PCB;


// Prototipos File system
void format(char bb[1024], char SB[2048], iNodo LI[5][16], char datos[50][1024], int *indiceLIL, int *indiceLBL);
void list(Dir directorio[64], iNodo listaInodos[5][16]);
int menu(void);
void create(char datos[][1024],iNodo listaInodos[][16], int *indiceLBL, int *indiceLIL, int *LIL, int *LBL,char isDir);
Dir *namei(char datos[][1024], iNodo listaInodos[][16], char *path);
void *getBlock(char datos[][1024], iNodo *inode, int blockIdx);
iNodo *getInode(iNodo listaInodos[][16], int inodo);
void prepend(char* s, const char* t);
void crearRaiz(char *bloque, iNodo listaInodos[][16]);
int load(char bb[1024], char SB[2048], iNodo LI[5][16], char datos[50][1024], int *indiceLIL, int *indiceLBL);
int delete(char datos[][1024],iNodo listaInodos[][16], int *indiceLBL, int *indiceLIL, int *LIL, int *LBL);

// Prototipos Round robin
void printList(int list[], int actual, int ultimo);
void imprimirEjeY(PCB tProcesos[], int cantidadProc);
void gotoxy(int x,int y);
int buscarIndice(int lista[], int pid, int len);
void roundRobin(void);

int main(){
  int LIL [15] = {3,4,5,6,7,8,9,10,11,12,13,14,15,16,17};
  int LBL [15] = {10,11,12,13,14,15,16,17,18,19,20,21,22,23,24};
  int opc = 0, indiceLIL = 0, indiceLBL = 0;
  char buffer[1024], contentBuffer[1025];
  iNodo *fileInode;
  Dir *file, *fileDir;

  //bloques
  // 1024 - 8 = 1016 bloques restantes
  // 4 + 12 = 16 bytes/archivo (struct dir)
  //1 bloque = 1024 bytes
  // 1024 / 16 bytes(archivo) = 64 archivos/bloque
  char boot[1024] = {0};
  char super[2048];
  // 50 bloques de 1024 bytes cada uno
  iNodo listaInodos[5][16];
  char datos[50][1024];

  if(load(boot, super, listaInodos, datos, &indiceLIL, &indiceLBL) == -1){
    printf("File System no encontrado, creando raiz...\n\n");
    crearRaiz(datos[0], listaInodos);
    format(boot, super, listaInodos, datos, &indiceLIL, &indiceLBL);
  } else {
    printf("File System encontrado, cargando...\n\n");
  }

  while(opc != -1){
    opc = menu();
    switch(opc){
      case 1:
        create(datos, listaInodos, &indiceLBL, &indiceLIL, LIL, LBL, 0);
        format(boot, super, listaInodos, datos, &indiceLIL, &indiceLBL);
        break;
      case 2:
        create(datos, listaInodos, &indiceLBL, &indiceLIL, LIL, LBL, 1);
        format(boot, super, listaInodos, datos, &indiceLIL, &indiceLBL);
        break;
      case 3:
        printf("\n------------------------\n\n");
        printf("Listando archivos en el directorio raiz\n");
        list((Dir *) datos[0], listaInodos);
        printf("\n\n");
        break;
      case 4:
        printf("Ingresa el nombre del archivo:\n");
        scanf("%s", buffer);
        if(buffer[0] != '/')
          prepend(buffer, "/");

        file = namei(datos, listaInodos, buffer);
        fileInode = getInode(listaInodos, file->iNodo);

        if(fileInode->type == 'd'){
          printf("%s Es un directorio, listando sus archivos:\n\n", buffer);
          fileDir = (Dir *) getBlock(datos, fileInode, 0);
          list(fileDir, listaInodos);
          printf("\n");
        } else if(fileInode->type == '-'){
          printf("\nContenido de: %s\n\n", buffer);
          printf("%s\n\n", (char *) getBlock(datos, fileInode, 0));
          printf("------------------------------\n\n");
        }
        break;
      case 5:
        delete(datos, listaInodos, &indiceLBL, &indiceLIL, LIL, LBL);
        format(boot, super, listaInodos, datos, &indiceLIL, &indiceLBL);
        break;
      case 6:
        roundRobin();
        break;
      case -1:
        break;
      default:
        printf("Opcion no soportada\n");
    } //fin switch
  }

  return 0;
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
}

int load(char bb[1024], char SB[2048], iNodo LI[5][16], char datos[50][1024], int *indiceLIL, int *indiceLBL){
  int fd;
  fd = open("Fs", O_RDONLY);
  if (fd==-1){
      perror("");
      return -1;
  }

  read(fd, bb, 1024);
  read(fd, SB, 2048);
  read(fd, LI, sizeof(iNodo)*5*16);
  read(fd, datos, 50 * 1024);
  read(fd, indiceLIL, sizeof(int));
  read(fd, indiceLBL, sizeof(int));
  close(fd);

  return 0;
}

void format(char bb[1024], char SB[2048], iNodo LI[5][16], char datos[50][1024], int *indiceLIL, int *indiceLBL){
    int fd;
    fd = open("Fs", O_WRONLY|O_CREAT);
    if (fd==-1){
        perror("");
        return;
    }

    write(fd, bb, 1024);
    write(fd, SB, 2048);
    write(fd, LI, sizeof(iNodo)*5*16);
    write(fd, datos, 50 * 1024);
    write(fd, indiceLIL, sizeof(int));
    write(fd, indiceLBL, sizeof(int));
    close(fd);
}

int menu(void){
  int opc;
  printf("Escoje una opcion\n\n");
  printf("1. Crear Archivo\n");
  printf("2. Crear Directorio\n");
  printf("3. Listar /\n");
  printf("4. Mostrar Contenidos de archivo\n");
  printf("5. Eliminar archivo\n");
  printf("6. Round Robin\n");
  printf("-1. Terminar Programa\n");
  scanf("%i", &opc);
  return opc;
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

void prepend(char* s, const char* t){
    size_t len = strlen(t);
    memmove(s + len, s, strlen(s) + 1);
    memcpy(s, t, len);
}

void crearRaiz(char *bloque, iNodo listaInodos[][16]){
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
    16216, //Last modified
    {9} // Content table
  };

  for(i=2; i<64; i++){
    root[i].iNodo = 0;
  }
  memcpy(bloque, root, sizeof(Dir) * 64);
  memcpy(&listaInodos[0][1], &inodoRaiz, sizeof(iNodo));
}

void printList(int list[], int actual, int ultimo){
  int i;
  printf("\nLista de ejecución:\n");
  for(i=0; i<=ultimo; i++){
    printf("%i", list[i]);
    if(i<ultimo)
      printf(", ");
  }
  printf("\n");
}

void imprimirEjeY(PCB tProcesos[], int cantidadProc){
  int i;
  for(i=0; i<cantidadProc; i++){
    if(tProcesos[i].estado != 0){
      printf("%i", i);
      if(i < 9){
        printf(" ");
      }
      printf("|\n");
    }
  }

}

void gotoxy(int x,int y){
    printf("%c[%d;%df",0x1B,y,x);
}

int buscarIndice(int lista[], int pid, int len){
  int i;
  for(i=0; i<len; i++){
    if(lista[i] == pid)
      return i;
  }
  return -1;
}

void roundRobin(){
  PCB tProcesos[10];
  int quantum = 2, i, j, actual = 0, ultimo = -1;
  int lista[10];
  int procActual = 0, fd;
  int tiempoTranscurrido = 0, tiempoProcActual = 0;
  int xOffset = 4, yOffset = 4;
  int listaInicial[10], lenListaInicial;

  printf("Esperando a recibir datos de la tabla de procesos...\n");
  fd = open("requests", O_RDONLY);
  if(fd == -1){
    perror("error al leer procesos");
    return;
  }

  read(fd, tProcesos, sizeof(PCB)*10);
  close(fd);

  system("clear");

  for(i=0; i<10; i++){
    if(tProcesos[i].estado != 0){
      ultimo++;
      lista[ultimo] = tProcesos[i].pid;
    }
  }
  memcpy(listaInicial, lista, sizeof(int) *10);
  lenListaInicial = ultimo+1;

  // lista = [0, 1, 3]
  //  actual  ^     ^ ultimo

  printf("Round Robin!!\nQuantum: %i", quantum);
  gotoxy(0, yOffset);
  imprimirEjeY(tProcesos, 10);

  while(actual <= ultimo){
    procActual = lista[actual];

    // Calcular tiempo del proceso actual
    if(tProcesos[procActual].tEjec < quantum){
      tiempoProcActual = tProcesos[procActual].tEjec;
    } else {
      tiempoProcActual = quantum;
    }

    gotoxy(tiempoTranscurrido + xOffset, buscarIndice(listaInicial, procActual, 10) + yOffset);
    for(j=0; j<tiempoProcActual; j++){
      printf("*");
    }

    printf("\n");

    tiempoTranscurrido += tiempoProcActual;
    tProcesos[procActual].tEjec -= tiempoProcActual;


    // Si al proceso que se acaba de atender
    // aun le queda tiempo de ejecución
    // ponerlo al final de la fila
    if(tProcesos[procActual].tEjec > 0){
      ultimo++;
      lista[ultimo] = tProcesos[procActual].pid;
    }

    actual++;
  }

  gotoxy(0,lenListaInicial+1+yOffset);
  printList(lista, actual, ultimo);
}
