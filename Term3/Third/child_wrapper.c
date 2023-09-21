#include <stdio.h>
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

	for (int i = 0; i < count; i++, argv += 2) {
		int pid = fork();
		switch (pid) {
			case 0:
				return child_main(3, argv);
			case -1:
				printf("Fail to fork\n");
				return -1;
			default:
				pids[i] = pid;
				break;
		}
	}
	for (int i = 0; i < count; i++) {
		int status = 0;
		int err = wait(&status);
		if (status < 0) {
			printf("Child return error: %d\n", WEXITSTATUS(status));
		} else {
			printf("Child done %d changes\n", WEXITSTATUS(status));
		}
	}

	return 0;
}