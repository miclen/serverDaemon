/*
 * =====================================================================================
 *       Filename:  serverdaemon.c
 *        Version:  1.0
 *        Created:  27/02/12 22:58:33
 *       Compiler:  gcc
 *         Author:  Anis Moubarik (), moubarik@cs.helsinki.fi
 * =====================================================================================
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "3490"

#define BACKLOG 10


int main(void)
{

	pid_t pid, sid;

	pid = fork();

	if(pid < 0){
		exit(EXIT_FAILURE);
	}else if (pid > 0){
		exit(EXIT_SUCCESS);
	}

	umask(0);

	sid = setsid();

	if(sid < 0){
		exit(EXIT_FAILURE);
	}

	if((chdir("/")) < 0){
		exit(EXIT_FAILURE);
	}

	while(1){
		sleep(10);
		printf("tic\n");
	}

	exit(EXIT_SUCCESS);
}
