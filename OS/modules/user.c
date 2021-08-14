#include "user.h"

int getOldestSessionAccess(Session *activeSessions){
  int i, oldestIndex;
  time_t oldestTime;

  oldestIndex = 0;
  oldestTime = activeSessions[0].lastAccess;
  for(i=0; i<MAX_SESSIONS; i++){
    if(activeSessions[i].lastAccess < oldestTime){
      oldestIndex = i;
      oldestTime = activeSessions[i].lastAccess;
    }
  }

  return oldestIndex;
}
