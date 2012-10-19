CC ?= gcc
# CFLAGS ?= -std=c99
CFDEBUG = -pedantic -Wall

EXEC = indexer
SRCS = indexer.c util.c hash.c main.c
OBJS = ${SRCS:.c=.o}

all: ${EXEC}

.c.o:
	${CC} ${CFLAGS} -o $@ -c $<

${EXEC}: ${OBJS}
	${CC} ${LDFLAGS} -o ${EXEC} ${OBJS}

debug: ${EXEC}
debug: CC += ${CFDEBUG}

test: debug
	./${EXEC} test_index test_index_dir

clean:
	rm -rf ./*.o
	rm -rf ./${EXEC}

.PHONY: all debug clean
