#ifndef SCHEDULER_H
#define SCHEDULER_H

// Longitud del comando abreviado que se mostrara en el reporte de RR
#define COMMAND_ABREV_LEN 8
// Tamaño del buffer de la fila del calendarizador
#define SCHEDULER_BUFFLEN 20
// Segundos entre peticiones para que se consideren como si hubieran llegado al
//mismo tiempo
#define TIME_BUFFER_SECS 5

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "utils.h"
#include "userActions.h"

typedef struct scheduledRequest {
  char command[COMMAND_ABREV_LEN];
  int executionTime;
  time_t time;
  int pid;
} ScheduledRequest;

typedef struct requestQueue {
  ScheduledRequest list[SCHEDULER_BUFFLEN];
  int start, end, capacity, size;
} RequestQueue;

int enqueueProcess(RequestQueue *queue, ScheduledRequest newReq);
ScheduledRequest dequeueProcess(RequestQueue *queue);
void scheduleRequest(RequestPacket *request, ResponsePacket *response);
void roundRobin();

#endif
