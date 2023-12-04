#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define IN_QUEUE 96
#define OUT_QUEUE 94

typedef struct {
	long type;
	char file[64], param[6];
} in_msg_type;

typedef struct {
	long type;
	int result;
} out_msg_type;

int child_main(int argc, char* argv[]);

int main(int argc, char* argv[]) {
	if (argc <= 1 || argc % 2 == 0) {
		printf("Arguments not specified or not complete\n");
		return -1;
	}

	int count = argc / 2;
	int pids[count];
	char** args = argv + 1;
	int symma = 0;
	in_msg_type message;

	int in_queue = msgget(IN_QUEUE, IPC_CREAT | 0666);
	int out_queue = msgget(OUT_QUEUE, IPC_CREAT | 0666);

	int result;
	for (int i = 0; i < count; i++) {
		strncpy(message.file, *args++, sizeof(message.file));
		strncpy(message.param, *args++, sizeof(message.param));
		msgsnd(in_queue, &message, 70, 0);
		int pid = fork();
		switch (pid) {
			case 0:
				in_msg_type message;
				msgrcv(in_queue, &message, 70, 0, 0);

				char* argv[3] = {0, message.file, message.param};
				result = child_main(3, argv);
				out_msg_type res_message = {1, result};
				msgsnd(out_queue, &res_message, sizeof(int), 0);
				return 0;

			case -1:
				printf("Fail to fork\n");
				return -1;
			default:
				pids[i] = pid;
				result = msgrcv(out_queue, &res_message, sizeof(int), 0, 0);
				symma = symma + res_message.result;
				printf("Child done %d changes\n", res_message.result);
				break;
		}
	}
	printf("%d", symma);
	return 0;
}
