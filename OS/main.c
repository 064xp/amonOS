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
#include "modules/fsDefinitions.h"
#include "modules/user.h"
#include "modules/userActions.h"
#include "modules/IPCtypes.h"
#include "modules/scheduler.h"

int main(){
  RequestPacket request;
  ResponsePacket response;
  int fdIn, fdOut;

  fsInit();
  initSessions();

  while(1){
    fdIn = open("input", O_RDONLY);
    fdOut = open("output", O_WRONLY);
    memset(response.buffer, 0, OUTPUT_BUF_LEN); // change to real buff len
    memset(response.cwd, 0, TOKENLEN); // change to real buff len
    memset(response.user, 0, 12);

    read(fdIn, &request, sizeof(RequestPacket));
    response.secCode = request.secCode;

    // Craft packet
    scheduleRequest(&request, &response);

    write(fdOut, &response, sizeof(ResponsePacket));

    close(fdIn);
    close(fdOut);
  }

  return 0;
}
