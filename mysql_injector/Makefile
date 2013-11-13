
CC=gcc
CFLAGS=-m32
AS=as
LD=ld

MAIN_HEADERS= moduleLogging.h \
              mysqlLog.h \
							errorCodes.h
MAIN_SOURCES= dbDetails.c \
              main.c      \
              mysqlLog.c  \
							errorCodes.c \
							moduleLogging.c 


MAIN_OBS=$(MAIN_SOURCES:.c=.o)

SRC_DIR=./ #provisional
BUILD_DIR=

INCLUDE_DIRS=
LIB_DIRS=

MYSQL_INCLUDE_FLAGS=-I/usr/include/mysql
MYSQL_LINK_FLAGS=-L/usr/lib/mysql -lmysqlclient

EXE=mysql_injector

all: $(EXE)
	@:

.PHONY: all clean install check test distclean


$(EXE): $(MAIN_OBS) $(MAIN_HEADERS)
	$(CC) $(CFLAGS) $(MYSQL_LINK_FLAGS) $(MAIN_OBS) -o $@

clean:
	-'rm' $(MAIN_OBS) $(TEST_OBS) $(EXE)

%.o: %.c
	$(CC) $(CFLAGS) $(MYSQL_INCLUDE_FLAGS) -c $<

