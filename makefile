GCC = gcc
DAEMON_BIN = serverDaemon
DAEMON_SRC = serverdaemon.c
CLIENT_BIN = client
CLIENT_SRC = client.c
FLAGS = -O3 -W

all: daemon client

daemon:
	${GCC} ${FLAGS} ${DAEMON_SRC} -o ${DAEMON_BIN}

client:
	${GCC} ${FLAGS} ${CLIENT_SRC} -o ${CLIENT_BIN}

clean:
	rm -f a.out *.core ${DAEMON_BIN} ${CLIENT_BIN}
