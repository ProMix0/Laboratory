#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h"

int process_args(char *arg);
void do_cleanup(int sig);

int main() {
	mkfifo(ARG_PIPE, 0666);
	mkfifo(RES_PIPE, 0666);

	int arg_fd = open(ARG_PIPE, O_RDONLY);
	if (arg_fd < 0) {
		fprintf(stderr, "Unable to open argument pipe\n");
		return -1;
	}
	int res_fd = open(RES_PIPE, O_WRONLY);
	if (res_fd < 0) {
		fprintf(stderr, "Unable to open result pipe\n");
		return -1;
	}

	signal(SIGTERM, do_cleanup);

	while (1) {
		fprintf(stderr, "Waiting for client connection\n");

		char buf[1024];
		int readed = 0;
		do {
			readed = read(arg_fd, buf, 1024);
		} while (readed == 0);
		fprintf(stderr, "Arguments received: \"%s\"\nStart processing\n", buf);

		int result = process_args(buf);
		fprintf(stderr, "Result: %d\n", result);

		fprintf(stderr, "Sending result\n");
		write(res_fd, &result, sizeof(int));
	}
}

void do_cleanup(int sig) {
	fprintf(stderr, "Stopping service\n");
	unlink(ARG_PIPE);
	unlink(RES_PIPE);
	exit(0);
}

int process_args(char *arg) {
	char fileName[256];
	char toDelete;

	if (sscanf(arg, "%s %c", fileName, &toDelete) != 2) {
		fprintf(stderr, "Unable to read args from memory\n");
		return -1;
	}

	int inputFile = open(fileName, O_RDONLY);
	if (inputFile < 0) {
		fprintf(stderr, "Fail to open file \"%s\"\n", fileName);
		return -1;
	}

	char destinationFileName[strlen(fileName) + 5];
	strcpy(destinationFileName, fileName);
	int outputFile = creat(strcat(destinationFileName, ".out"),
						   S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (outputFile < 0) {
		fprintf(stderr, "Fail to write to \"%s\"\n", destinationFileName);
		return -1;
	}

	int deletions = 0;
	int c = 0;
	while (read(inputFile, &c, 1)) {
		if (c != toDelete) {
			write(outputFile, &c, 1);
		} else {
			deletions++;
		}
	}

	close(inputFile);
	close(outputFile);

	return deletions;
}
