#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
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
	mkfifo("get_task", 0666);
	mkfifo("report_task", 0666);

	for (int i = 0; i < count; i++) {
		int pid = fork();
		int fdIn = -1;
		int fdOut = -1;
		switch (pid) {
			case 0:
				fdIn = open("get_task", O_RDONLY);
				if (fdIn < 0) {
					printf("Fail to open or create first pipe\n");
					return -1;
				}
				fdOut = open("report_task", O_WRONLY);
				if (fdOut < 0) {
					printf("Fail to open or create second pipe\n");
					return -1;
				}
				dup2(fdIn, STDIN_FILENO);
				dup2(fdOut, STDOUT_FILENO);
				return child_main(1, argv);
			case -1:
				printf("Fail to fork\n");
				return -1;
			default:
				pids[i] = pid;

				fdIn = open("get_task", O_WRONLY);
				if (fdIn < 0) {
					printf("Fail to open or create first pipe\n");
					return -1;
				}
				fdOut = open("report_task", O_RDONLY);
				if (fdOut < 0) {
					printf("Fail to open or create second pipe\n");
					return -1;
				}

				write(fdIn, *args, strlen(*args));
				args++;

				char space = ' ';
				write(fdIn, &space, 1);

				write(fdIn, *args, strlen(*args));
				args++;
				close(fdIn);

				char out[64];
				int len = read(fdOut, out, 63);
				out[len] = 0;
				close(fdOut);

				printf("Child done %d changes\n", atoi(out));
				break;
		}
	}

	return 0;
}