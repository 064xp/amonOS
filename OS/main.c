/****
Equipo:
  Paulo Eduardo Zamora Vanegas
  Gerardo Castruita Lopez
****/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "modules/lexer.h"
#include "modules/filesystem.h"
#include "modules/user.h"

typedef struct PCB {
  char estado;
  int pid;
  char progCounter;
  int tEjec;
} PCB;

// Prototipos Round robin
void printList(int list[], int actual, int ultimo);
void imprimirEjeY(PCB tProcesos[], int cantidadProc);
void gotoxy(int x,int y);
int buscarIndice(int lista[], int pid, int len);
void roundRobin(void);

int main(){
  char outputBuffer[OUTPUT_BUF_LEN];
  char cmd[500];
  Session rootUsr = {"root", "/"};

  fsInit();

  while(1){
    gets(cmd);
    executeCommand(outputBuffer, &rootUsr, cmd);
    printf("%s\n", outputBuffer);
  }
  return 0;
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
