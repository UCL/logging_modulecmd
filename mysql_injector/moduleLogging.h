#ifndef INC_MODULELOGGING_H

#define maxModuleLength 1024

typedef struct ModuleLogEvent_s {
  char username[16];
  char hostname[16];
  unsigned int pid;
  unsigned int jobid;
  int taskid;
  char event[16];
  char module[maxModuleLength];
} ModuleLogEvent_t;

extern const int ERR_INVALID_USERNAME;
extern const int ERR_INVALID_HOSTNAME;
extern const int ERR_INVALID_EVENT   ;

#define INC_MODULELOGGING_H
#endif
