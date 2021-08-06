#include <string.h>
#include <stdio.h>
#include <string.h>

// Tamaño máximo de cada token del comando
#define TOKENLEN 25
// Cantidad de comandos que se tienen
#define COMMANDSLEN 1
// Cantidad máxima de argumentos de un comando
#define ARG_AMMOUNT 5

void separateCommand(char *command, int *argc, char argv[][TOKENLEN]);
int execute(int argc, char argv[][TOKENLEN]);
void executeCommand(char *command);
