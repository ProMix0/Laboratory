#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int child_main(int argc, char* argv[]);

int main(int argc, char* argv[]) {
	if (argc <= 1 || argc % 2 == 0) {
		printf("Arguments not specified or not complete\n");
		return -1;
	}

	int count = argc / 2;
	int pids[count];
	char** args = argv + 1;

	for (int i = 0; i < count; i++) {
		int pipeIn[2];
		pipe(pipeIn);
		int pipeOut[2];
		pipe(pipeOut);
		int pid = fork();
		switch (pid) {
			case 0:
				close(pipeIn[1]);
				close(pipeOut[0]);
				dup2(pipeIn[0], STDIN_FILENO);
				dup2(pipeOut[1], STDOUT_FILENO);
				int ret = child_main(1, argv);
				printf("Child returned %d\n", ret);
				return ret;
			case -1:
				printf("Fail to fork\n");
				return -1;
			default:
				pids[i] = pid;
				close(pipeIn[0]);
				close(pipeOut[1]);

				write(pipeIn[1], *args, strlen(*args));
				args++;

				char space = ' ';
				write(pipeIn[1], &space, 1);

				write(pipeIn[1], *args, strlen(*args));
				args++;
				close(pipeIn[1]);

				char out[64];
				int len = read(pipeOut[0], out, 63);
				out[len] = 0;
				close(pipeOut[0]);

				printf("Child done %d changes\n", atoi(out));
				break;
		}
	}

	return 0;
}