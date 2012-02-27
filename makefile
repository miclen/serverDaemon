GCC = gcc
DAEMON_BIN = serverDaemon
DAEMON_SRC = serverdaemon.c
FLAGS = -O3 -W

all: daemon

daemon:
	${GCC} ${FLAGS} ${DAEMON_SRC} -o ${DAEMON_BIN}

clean:
	rm -f a.out *.core ${DAEMON_BIN}
