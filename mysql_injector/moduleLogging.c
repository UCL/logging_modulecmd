#include "moduleLogging.h"
#include "errorCodes.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>

int validStringFields( ModuleLogEvent_t *ModuleLogEvent ) {

  // Check username, hostname, and event
  if (! validUsername(ModuleLogEvent->username) ) {
    fprintf(stderr, "Error: invalid username.\n");
    return ERR_INVALID_USERNAME;
  }
  if (! validHostname(ModuleLogEvent->hostname) ) {
    fprintf(stderr, "Error: invalid hostname.\n");
    return ERR_INVALID_HOSTNAME;
  }
  if (! validEvent(ModuleLogEvent->event) ) {
    fprintf(stderr, "Error: invalid event type.\n");
    return ERR_INVALID_EVENT;
  }
}

int validUsername(const char *username) {
  int i;
  for (i=0; i<strlen(username); i++) {
    if (! isalnum(username[i]) ) {
      return 0;
    }
  }
  return 1;
}

int validHostname(const char *hostname) {
  int i;
  for (i=0; i<strlen(hostname); i++) {
    if (! (isalnum(hostname[i]) || ( hostname[i] == '-' ) ) ) {
      return 0;
    }
  }
  return 1;
}

int validEvent(const char *event) {
  if ( (0 == strcmp(event, "Load")   ) ||
       (0 == strcmp(event, "Unload") ) ) 
  {
    return 1;
  } else {
    return 0;
  }
}




