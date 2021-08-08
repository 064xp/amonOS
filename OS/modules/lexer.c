#include "lexer.h"

// Lista de comandos
Command commands [COMMANDSLEN] = {
  {"ls", &ls}
};

/*
  Separa el comando en cada espacio.
  - char *command: Comando que se va a separar
  - int *argc (argument counter): Cantidad de argumentos (contando
    al comando).
  - char argv[][] (argument vector): Arreglo con los argumentos

  ej. cp arch1 dir1
    argc = 3
    argv = ["cp", "arch1", "dir1"]
*/
void separateCommand(char *command, int *argc, char argv[][TOKENLEN]){
  int i, count = 0;

  *argc = 0;
  for(i=0; i<=strlen(command); i++){
    if(command[i] == ' ' || command[i] == '\0'){
      argv[*argc][count] = '\0';
      *argc = *argc + 1;
      count = 0;
      continue;
    }
    argv[*argc][count] = command[i];
    count++;
  }
}

/*
  Toma argc y argv, busca el comando en la lista de comandos
  y ejecuta la función correspondiente al comando.

  Pasa a esa función argc y argv

  > Regresa el valor de retorno de la función en caso que se haya encontrado
  > Regresa -1 si no se encontró el comando

*/
int execute(char *outputBuffer, int argc, char argv[][TOKENLEN]){
  int i, retVal;

  for(i=0; i<COMMANDSLEN; i++){
    if(strcmp(argv[0], commands[i].name) == 0){
      retVal = commands[i].function(outputBuffer, argc, argv);
      return retVal;
    }
  }

  return -1;
}


/*
  Función que debe ser llamada desde afuera

  Recibe el comando completo a ejecutar, lo separa en argumentos
  y llama la función correspondiente con sus argumentos.
*/
void executeCommand(char *outputBuffer, char *command){
  int argc, error;
  char argv[ARG_AMMOUNT][TOKENLEN];

  separateCommand(command, &argc, argv);

  error = execute(outputBuffer, argc, argv);
  if(error == -1){
    printf("[!] Command not found!!\n");
  }

}
