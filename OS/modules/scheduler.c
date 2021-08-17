#include "scheduler.h"

RequestQueue requests = {{}, 0, -1, SCHEDULER_BUFFLEN, 0};

void scheduleRequest(RequestPacket *request, ResponsePacket *response){
  ScheduledRequest newReq;
  time_t curTime = getCurrentTime();
  srand(time(0));

  // Numero aleatorio del 1 al 5
  newReq.executionTime = (rand() % (5 - 1 + 1)) + 1;
  memcpy(newReq.command, request->buffer, COMMAND_ABREV_LEN -1);
  newReq.command[COMMAND_ABREV_LEN-1] = '\0';
  newReq.time = curTime;
  newReq.pid = requests.size;

  // Si la diferencia entre el nuevo req y el ultimo que tenemos es > 5 segs
  if(curTime - requests.list[requests.end].time > TIME_BUFFER_SECS){
    roundRobin();
    newReq.pid = 0;
  }
  enqueueProcess(&requests, newReq);
  executeAuthenticated(request, response);
}


void roundRobin(){
  char outputBuffer[1024] = "", tempBuff[100];
  int i, quantum = 2, fd;
  int timeElapsed = 0, currentProcTime = 0;
  ScheduledRequest currentProcess;
  time_t currentTime;

  // Header
  if(requests.size == 0)
    return;

  time(&currentTime);

  sprintf(outputBuffer, "===>>> Round Robin >> %s \nQuantum: %i\n\n", ctime(&currentTime), quantum);
  strcat(outputBuffer, "Process Table:\n\n");
  strcat(outputBuffer, " PID\tProcess\t\tExecution Time\n");
  for(i=0; i<requests.size;  i++){
    sprintf(tempBuff, " %i\t%s\t\t%i\n", requests.list[i].pid, requests.list[i].command, requests.list[i].executionTime);
    strcat(outputBuffer, tempBuff);
  }

  strcat(outputBuffer, "\n\nExecution List\n\n");
  while(requests.size > 0){
    currentProcess = requests.list[requests.start];

    if(currentProcess.executionTime < quantum){
      currentProcTime = currentProcess.executionTime;
    } else {
      currentProcTime = quantum;
    }

    for(i=0; i<currentProcTime; i++){
      sprintf(tempBuff, "[%i]", currentProcess.pid);
      strcat(outputBuffer, tempBuff);
    }


    timeElapsed += currentProcTime;
    requests.list[requests.start].executionTime -= currentProcTime;

    // Si al proceso que se acaba de atender
    // aun le queda tiempo de ejecución
    // ponerlo al final de la fila
    if(requests.list[requests.start].executionTime > 0){
      //Agregarlo al final
      enqueueProcess(&requests, requests.list[requests.start]);
    }
    // Quitarlo del principio
    dequeueProcess(&requests);

    if(requests.size > 0)
      strcat(outputBuffer, " -> ");
  }
  strcat(outputBuffer, "\n\n");

  // Escribir a log
  fd = open("rr.log", O_CREAT|O_WRONLY|O_APPEND);
  write(fd, outputBuffer, strlen(outputBuffer));
  close(fd);
}

int enqueueProcess(RequestQueue *queue, ScheduledRequest newReq){
    if(queue->size == queue->capacity){
        return 1;
    }

    queue->end = (queue->end + 1) % queue->capacity;
    memcpy(&queue->list[queue->end], &newReq, sizeof(ScheduledRequest));
    queue->size++;
    return 0;
}

ScheduledRequest dequeueProcess(RequestQueue *queue){
    ScheduledRequest element = {
      "",
      -1,
      -1
    };

    if(queue->size == 0){
        return element;
    }
    element = queue->list[queue->start];
    queue->start = (queue->start+1) % queue->capacity;
    queue->size--;
    if(queue->size == 0){
      queue->start = 0;
      queue->end = -1;
    }
    return element;
}
