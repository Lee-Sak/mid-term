#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCK_PATH "echo_socket"
#define MAXSIZE 1024

int main(void) {
	int s, t, len;;
	struct sockaddr_un  remote;
	char str[100];
	char line[MAXSIZE];

	if( (s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1 ) {
		printf("socket error");
		exit(1);
	}
	
	printf("Trying to connect...\n");

	remote.sun_family = AF_UNIX;
	strcpy(remote.sun_path, SOCK_PATH);
	len = strlen(remote.sun_path) + sizeof(remote.sun_family);
	
	if(connect(s, (struct sockaddr *)&remote, len) == -1) {
		printf("connect");
		exit(1);
	}

	printf("Connected.\n");

	while(printf("> "), fgets(str, 100, stdin), !feof(stdin)) {
		if(send(s, str, strlen(str), 0) == -1) {
			printf("send");
			exit(1);
		}

		if(( t=recv(s, line, 100, 0) ) > 0) {
			line[t] = '\0';
			printf("echo> %s", line);
			printf("\n");
		}
		else {
			if(t < 0) printf("recv");
			else printf("server closed connection!\n");
			exit(1);
		}
	}
	close(s);
	return 0;
}
