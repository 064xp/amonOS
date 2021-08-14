#ifndef MACROS_H
#define MACROS_H
// Tamaño máximo de cada token del comando
#define TOKENLEN 256
// Cantidad total de bloques en el disco
#define TOTALBLKS 58
// Tamaño del buffer de salida
#define OUTPUT_BUF_LEN 3072
// Cantidad maxima de usuarios
#define TOTALUSERS 32
// Cantidad de sessiones simultaneas maximas
#define MAX_SESSIONS 15

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#endif
