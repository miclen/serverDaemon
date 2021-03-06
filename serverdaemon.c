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
#include <syslog.h>

#define PORT "3490"
#define DEFAULT_LOGFLAG 0
#define BACKLOG 10
#define DAEMON_NAME "serverDaemon"
#define PID_FILE "serverDaemon.pid"

void signal_handler(int sig)
{
	switch(sig){
		case SIGHUP:
					syslog(LOG_WARNING, "Received SIGHUP signal.");
					break;
		case SIGTERM:
					syslog(LOG_WARNING, "Received SIGTERM signal.");
					break;
		default:
					syslog(LOG_WARNING, "Unhandled signal (%d) %s", strsignal(sig));
					break;
	}
}

void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

void *get_in_addr(struct sockaddr *sa)
{
	if(sa->sa_family == AF_INET){
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char **argv)
{
	
	signal(SIGHUP, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, signal_handler);
	static int ch, logflag;
	pid_t pid = 0, sid;

	logflag = DEFAULT_LOGFLAG;
	


	while((ch = getopt(argc, argv, "l:")) != -1){
		switch(ch){
			case 'l':
							logflag = 1;
							break;
		}
	}
	pid = fork();
	printf("\nStarting process: %d", (int)pid);
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
	
	if(logflag == 1)
		syslog(LOG_NOTICE, " started by User %d", getuid());
	
	
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	while(1){
		int sockfd, new_fd; //Listen on sock_fd, new conn on new_fd
		struct addrinfo hints, *servinfo, *p;
		struct sockaddr_storage their_addr; //connectors info
		socklen_t sin_size;
		struct sigaction sa;
		int yes = 1;
		char s[INET6_ADDRSTRLEN];
		int rv;

		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;

		if((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0){
			fprintf(stderr, "getaddrinfo : %s\n", gai_strerror(rv));
			return 1;
		}

		for(p = servinfo; p != NULL; p = p->ai_next){
			if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
				perror("\nserver: socket");
				continue;
			}

			if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
				perror("\nsetsockopt");
				exit(1);
			}

			if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
				close(sockfd);
				perror("\nserver: bind");
				continue;
			}
			break;
		}

		if(p == NULL){
			fprintf(stderr, "server: failed to bind\n");
			return 2;
		}

		freeaddrinfo(servinfo); //all done with this struct

		if(listen(sockfd, BACKLOG) == -1){
			perror("listen");
			exit(1);
		}

		sa.sa_handler = sigchld_handler; // reap all dead processes
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_RESTART;
		if(sigaction(SIGCHLD, &sa, NULL) == -1){
			perror("\nsigaction");
			exit(1);
		}

		printf("\nserver: waiting for connections...\n");
		
		while(1){
			sin_size = sizeof their_addr;
			new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
			if(new_fd == -1){
				perror("\naccept");
				continue;
			}

			inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
			printf("\nserver: got connection from %s\n", s);

			if(!fork()){
				close(sockfd);
				if(send(new_fd, "Hello, World!", 13, 0) == -1)
					perror("send");
				close(new_fd);
				exit(0);
			}
			close(new_fd);
		}
	}

	exit(EXIT_SUCCESS);
}
