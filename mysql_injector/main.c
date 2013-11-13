#include <stdio.h>
#include <malloc.h>
#include "moduleLogging.h"
#include "mysqlLog.h"

int main(int argc, char **argv) {
  ModuleLogEvent_t *event;

  event = (ModuleLogEvent_t *) malloc(sizeof(ModuleLogEvent_t));

  char buffer[1024];
  int itemsMatched;
  int result;

  itemsMatched = fscanf(stdin,
                   "%s\n" //username
                   "%s\n" //hostname
                   "%u\n" //pid
                   "%u\n" //jobid
                   "%d\n" //taskid
                   "%s\n" //event
                   "%s\n", // module
                   event->username,
                   event->hostname,
                   &(event->pid),
                   &(event->jobid),
                   &(event->taskid),
                   event->event,
                   event->module );

  if ( itemsMatched == 7 )
  {
    result = LogEventInDB(event);
    if (result == LOG_SUCCESSFUL) 
    {
      return 0;
    }
  } else {
    fprintf(stderr, "Error: fscanf could not read in a full event spec: items matched = %d.\n", itemsMatched);
    return 2;
  }
  
}


