LIBS= inc.o errors.o
CC=gcc
PROG=pip crew
THREAD=-lpthread
all:${PROG}

pip:pipe.o ${LIBS}
	${CC}  -o $@ pipe.o ${LIBS} ${THREAD}
crew:crew.o ${LIBS}
	${CC} -o $@ crew.o ${LIBS} ${THREAD}


