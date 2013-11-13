#include <stdio.h>
#include <my_global.h>
#include <mysql.h>
#include <malloc.h>
#include <string.h>
#include "moduleLogging.h"
#include "mysqlLog.h"
#include "errorCodes.h"

int LogEventInDB( ModuleLogEvent_t *ModuleLogEvent ) {
  MYSQL     *databaseConnection;
  MYSQL_RES *databaseResults;
  MYSQL_ROW  databaseResultRow;
  int        numberOfRowFields;
  int        databaseReturnCode;
 
  char      *queryText;

  databaseConnection = mysql_init(NULL);

  const int timeout = 3; //seconds
  mysql_options(databaseConnection, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
  mysql_options(databaseConnection, MYSQL_OPT_READ_TIMEOUT   , &timeout);
  mysql_options(databaseConnection, MYSQL_OPT_WRITE_TIMEOUT  , &timeout);


  //MYSQL *mysql_real_connect(MYSQL *mysql, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, unsigned long client_flag)
  if ( NULL == mysql_real_connect(databaseConnection, 
                                  databaseHost, 
                                  databaseUser, 
                                  databasePassword, 
                                  databaseName, 
                                  databasePort, 
                                  NULL, 
                                  CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS) ) 
  {
    //Error determination not yet implemented because I haven't read that part of the manual yet. ._.
    fprintf(stderr, "Error: could not open database connection.\n");
    return ERR_COULDNOTLOG;
  }

  // We skip timestamp because mysql gens it automatically
  const char insertTemplate[] = "INSERT INTO %s (username, hostname, pid, jobid, taskid, event, module) "
                                "  VALUES (          '%s',     '%s',  %u,    %u,     %d,  '%s',   '%s') ";

  prepareModuleStrings(databaseConnection, ModuleLogEvent);

  if ( -1 == asprintf (&queryText, 
                       insertTemplate, 
                       databaseLogTable,
                       ModuleLogEvent->username,
                       ModuleLogEvent->hostname,
                       ModuleLogEvent->pid,
                       ModuleLogEvent->jobid,
                       ModuleLogEvent->taskid,
                       ModuleLogEvent->event,
                       ModuleLogEvent->module) )
  {
    fprintf(stderr, "Error: could not allocate query string. Out of memory?\n");
    exit(1);
  }

  databaseReturnCode = mysql_query(databaseConnection, queryText);
  free(queryText);
  // The above does do some checking and string escaping, but possibly not enough -- consider using stored procedure
  // See here for a code example: http://dev.mysql.com/doc/refman/5.5/en/c-api-prepared-call-statements.html

  mysql_close(databaseConnection);
  
  if ( databaseReturnCode != 0 )
  {
    fprintf(stderr, "Error: could not write event to database successfully.\n");
    return ERR_COULDNOTLOG;
  } else {
    return LOG_SUCCESSFUL;
  }
}


void prepareModuleStrings(MYSQL *databaseConnection, ModuleLogEvent_t *ModuleLogEvent) {
  unsigned long stringLength = strlen(ModuleLogEvent->module);
  char *buffer = (char *) malloc(stringLength * 2 + 1);
  int returnCode;

  if (! (returnCode = validStringFields(ModuleLogEvent) ) ){
    exit(returnCode);
  }
  
  // Escape the module string in case of hostile input.
  returnCode = (int) mysql_real_escape_string(databaseConnection, buffer, ModuleLogEvent->module, stringLength);

  if (strlen(buffer) + 1 <= maxModuleLength) {
    strcpy(ModuleLogEvent->module, buffer);
  } else {
    fprintf(stderr, "Error: escaped module string too large for buffer!\n");
    exit(ERR_ESCAPED_STRING_TOO_LARGE);
  }

  free(buffer);
}





/* This is more to show the table prototype than because we actually expect to ever have to do this. */
int LogDBInit() {
  MYSQL *databaseConnection;
  int    databaseReturnCode;
  char  *queryText;

  databaseConnection = mysql_init(NULL);

  if ( NULL == mysql_real_connect(databaseConnection, 
                                  databaseHost, 
                                  databaseUser, 
                                  databasePassword, 
                                  databaseName, 
                                  databasePort, 
                                  NULL,  // Optional socket argument
                                  0 ) )  // Optional client flags
  {
    //Error determination not yet implemented because I haven't read that part of the manual yet.
    fprintf(stderr, "Error: could not open database connection.\n");
    return 1;
  }

  const char creationTemplate[] = "CREATE TABLE %s( "
                                    "timestamp TIMESTAMP(),"
                                    "username  VARCHAR(16),"
                                    "hostname  VARCHAR(16),"
                                    "pid       INT,"
                                    "jobid     INT,"
                                    "taskid    INT,"
                                    "event     VARCHAR(16),"
                                    "module    TEXT );";
  
  if (-1 == asprintf(&queryText, creationTemplate, databaseName))
  {
    fprintf(stderr, "Error: could not allocate query string. Out of memory?\n");
    exit(1);
  }

  databaseReturnCode = mysql_query(databaseConnection, queryText);
  free(queryText);

  mysql_close(databaseConnection);

  if ( databaseReturnCode != 0 )
  {
    fprintf(stderr, "Error: could not create table.\n");
    return SQL_ERR_NOCREATETABLE;
  } else {
    return SQL_CREATE_SUCCESS;
  }
}

int TestDBConnection() {
  MYSQL *databaseConnection;
  int    databaseReturnCode;
  char  *errorReason;

  databaseConnection = mysql_init(NULL);

  if ( NULL == mysql_real_connect(databaseConnection, 
                                  databaseHost, 
                                  databaseUser, 
                                  databasePassword, 
                                  databaseName, 
                                  databasePort, 
                                  NULL,  // Optional socket argument
                                  0 ) )  // Optional client flags
  {
    //Error determination not yet implemented because I haven't read that part of the manual yet.
    fprintf(stderr, "Error: could not open database connection.\n");
    errorReason = (char *)mysql_error(databaseConnection);
    fprintf(stderr, "Reason: %s\n", errorReason);
    return 1;
  }
  mysql_close(databaseConnection);

  return 0;
}


