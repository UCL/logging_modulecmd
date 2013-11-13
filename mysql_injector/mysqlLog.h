#include "moduleLogging.h"
#include <my_global.h>
#include <mysql.h>
#include "errorCodes.h"

#ifndef INC_MODULELOGGING_MYSQLLOG_H


extern const char databaseHost[]     ;
extern const char databaseUser[]     ;
extern const char databasePassword[] ;
extern const char databaseName[]     ;
extern const char databaseLogTable[] ;
extern const unsigned int databasePort ;

int LogEventInDB( ModuleLogEvent_t *ModuleLogEvent);
void prepareModuleStrings(MYSQL *databaseConnection, ModuleLogEvent_t *ModuleLogEvent);

#define INC_MODULELOGGING_MYSQLLOG_H
#endif
