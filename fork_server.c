#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <ctype.h>

#define SOCK_PATH "echo_socket"
#define MAXLINE 1024

double op(char *a) {
	int x,y;
	char o;
	double result;
	x = atoi(&a[0]);
	o = a[1];
	y = atoi(&a[2]);

	if(o == '+')	result = x + y;
	else if(o == '-')	result = x - y;
	else if(o == '*')	result = x * y;
	else	result = (double)x / y;

	return result;
}

int main(void) {
	int s, s2, len;
	unsigned t;
	struct sockaddr_un local, remote;
	char str[100];
	int n, p2c[2],c2p[2];
	char line[MAXLINE];
	double res;
	pid_t pid;
	if( (s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1 ) {
		printf("socket error");
		exit(1);
	}

	local.sun_family = AF_UNIX;
	strcpy(local.sun_path, SOCK_PATH);
	unlink(local.sun_path); // 이미 해당 소켓이 있을 수 있으므로 제거
	len = strlen(local.sun_path) + sizeof(local.sun_family);
	
	// 소켓과 프로세스 연결, unlink안하면 바인딩이 되지 않음
	if(bind(s, (struct sockaddr *)&local, len) == -1) {
		printf("bind");
		exit(1);
	}
	
	// 서버가 수신 클라이언트 연결 대기
	if(listen(s, 5) == -1) {
		printf("listen");
		exit(1);
	} 

	for(;;) {
		int done, n;
		printf("waiting for a connection..\n");
		t = sizeof(remote);
		
		//클라이언트 접속 요청 승인
		if( (s2 = accept(s, (struct sockaddr *)&remote, &t)) == -1 ) {
			printf("accept");
			exit(1);
		}
		printf("Connected.\n");
		
		done = 0;
		
		do {
			n = recv(s2, line, 100, 0);
			if(n <= 0) {
				if(n < 0) printf("recv");
				done = 1;
			}
			if(!done) {
				if( pipe(p2c) < 0 || pipe(c2p) < 0)
					printf("error");
				if( (pid = fork()) < 0) 
					printf("error");
				else if( pid > 0 ) { // parent
					close(p2c[0]); // p2c read mode close
					close(c2p[1]); // c2p write mode close
					write(p2c[1], line, 14);
					n = read(c2p[0], line, MAXLINE);
					if(send(s2, line, n, 0) < 0) {
						printf("send");
						done = 1;
					}
				}
				else { // child
					close(p2c[1]);
					close(c2p[0]);
					n = read(p2c[0], line, MAXLINE);
					res = op(line);
					sprintf(line, "%.2lf", res);
					write(c2p[1], line, n);
				}
			}
		} while(!done);

		close(s2);
	}
	
	return 0;
}

