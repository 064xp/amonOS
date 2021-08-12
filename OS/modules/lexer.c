#include "lexer.h"

// Lista de comandos
Command commands [COMMANDSLEN] = {
  {"ls", &ls},
  {"touch", &touch},
  {"mkdir", &mkdir},
  {"rm", &rm},
  {"cd", &cd},
  {"pwd", &pwd},
  {"writef", &writef}
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
void separateCommand(char *command, int *argc, char (*argv)[TOKENLEN]){
  int i, count;

  *argc = 0;
  count = 0;
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
int execute(char *outputBuffer, User *user, int argc, char argv[][TOKENLEN]){
  int i, retVal;

  for(i=0; i<COMMANDSLEN; i++){
    if(strcmp(argv[0], commands[i].name) == 0){
      retVal = commands[i].function(outputBuffer, user, argc, argv);
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
void executeCommand(char *outputBuffer, User *user, char *command){
  int argc, error;
  char (*argv)[TOKENLEN];

  trim(command);
  argv = (char(*)[TOKENLEN]) mallocArgv(command);

  separateCommand(command, &argc, argv);

  error = execute(outputBuffer, user, argc, argv);
  if(error == -1){
    sprintf(outputBuffer, "Command \"%s\" not found", command);
  }

  free(argv);
}

/*
  Quita espacios al principio y al final de la cadena
*/
void trim(char *buffer){
  int i, counter = 0, len = strlen(buffer);
  char *temp = malloc(len);

  // Quitar espacios de el inicio
  for(i=0; i<len; i++){
    if(buffer[i] == ' ')
      counter++;
    else
      break;
  }

  strcpy(buffer, buffer+i);

  // Quitar espacios del final
  len = strlen(buffer);
  counter = 0;
  for(i=len-1; i>=0; i--){
    if(buffer[i] == ' ')
      counter++;
    else
      break;
  }
  memcpy(temp, buffer, len-(counter));
  temp[len-counter] = '\0';
  strcpy(buffer, temp);
  free(temp);
}

/*
  Toma una ruta y separa entre la ruta del padre y
  el nombre del archivo

  ej.
  /padre/archivo
  parent = /padre
  file = archivo
*/
void separeParentPath(char *fullPath, char *parent, char *file){
  int i, len = strlen(fullPath);

  if(fullPath[len-1] == '/')
    i = len -2;
  else
    i = len -1;

  for(; i>=0; i--){
    if(fullPath[i] == '/')
      break;
  }
  i++;

  if(file != NULL)
    strcpy(file, fullPath+i);

  if(i == 0)
    strcpy(parent, "");
  else{
    memcpy(parent, fullPath, i);
    parent[i] = '\0';
  }
}

char **mallocArgv(char *command){
    char (*argv)[TOKENLEN];
    int count = 0, i;

    // Contar espacios
    for(i=0; i<strlen(command); i++){
      if(command[i] == ' ')
        count++;
    }
    argv = malloc(sizeof(char[count][TOKENLEN]));

    return (char **)argv;
}

void prepend(char* s, const char* t){
    size_t len = strlen(t);
    memmove(s + len, s, strlen(s) + 1);
    memcpy(s, t, len);
}
