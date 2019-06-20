// Sending messages into the queue

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define BUFF_SIZE 1024

typedef struct {
	long data_type;
	int data_num;
	char data_buff[BUFF_SIZE];
} msg_t;

int main(void) {
	int msqid, msqid1;
	int n = 0;
	msg_t data;

	if(-1 == (msqid = msgget( (key_t)151515, IPC_CREAT | 0666 ))) {
		printf("msgget() error");
		exit(1);
	}
	if(-1 == (msqid1 = msgget( (key_t)515151, IPC_CREAT | 0666 ))) {
		printf("msgget() error");
		exit(1);
	}
	
	
	
	while(1) {
		data.data_type = (n++ % 3) + 1; // data_type = {1, 2, 3}
		data.data_num = n;
		
		//data_buff에 " " 부분 저장
		sprintf(data.data_buff, "type=%ld, n=%d", data.data_type, n);

		if(-1 == msgsnd(msqid, &data, sizeof(msg_t) - sizeof(long), 0)) {
			printf("msgsnd() error");
			exit(1);
		}
		else 
			printf("send 1\n");
		if(-1 == msgsnd(msqid1, &data, sizeof(msg_t) - sizeof(long), 0)) {
			printf("msgsnd() error");
			exit(1);
		}
		else 
			printf("send 2\n");

		sleep(1);
	}
	return 0;
}
