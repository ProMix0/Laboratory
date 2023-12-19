#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <unistd.h>

#include "common.h"

int process_args(char *arg);
void do_cleanup(int sig);

char *map_mem;
int queue;

int main() {
	int sh_file =
		open(SH_FILE, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (sh_file < 0) {
		fprintf(stderr, "Unable to open shared file: \"" SH_FILE "\"\n");
		return -1;
	}
	if (ftruncate(sh_file, 1024)) {
		fprintf(stderr, "Unable to truncate shared file: \"" SH_FILE "\"\n");
		return -1;
	}
	map_mem = mmap(0, 1024, PROT_WRITE | PROT_READ, MAP_SHARED, sh_file, 0);
	if (map_mem == MAP_FAILED) {
		fprintf(stderr, "Unable to map shared file\n");
		return -1;
	}

	queue = msgget(MSG_QUEUE, IPC_CREAT | 0666);
	if (queue < 0) {
		fprintf(stderr, "Unable to open message queue\n");
		return -1;
	}
	message_t read_result = {.type = READ_RESULT};

	signal(SIGTERM, do_cleanup);

	while (1) {
		fprintf(stderr, "Waiting for client connection\n");
		msgrcv(queue, 0, 0, WRITE_REQUEST, 0);
		fprintf(stderr, "Client connected\n");
		msgrcv(queue, 0, 0, WRITE_ACK, 0);

		fprintf(stderr, "Arguments received: \"%s\"\nStart processing\n",
				map_mem);
		int result = process_args(map_mem);
		*((int *)map_mem) = result;
		fprintf(stderr, "Result: %d\n", result);

		fprintf(stderr, "Sending result\n");
		msgsnd(queue, &read_result, 0, 0);
		fprintf(stderr, "Client connection ended\n");
		msgrcv(queue, 0, 0, READ_DONE, 0);
	}
}

void do_cleanup(int sig) {
	fprintf(stderr, "Stopping service\n");
	unlink(SH_FILE);
	munmap(map_mem, 1024);
	msgctl(queue, IPC_RMID, 0);
	exit(0);
}

int process_args(char *arg) {
	char fileName[256];
	int limit;

	if (sscanf(arg, "%s %d", fileName, &limit) != 2) {
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

	int replace_count = 0;
	int first = 0;
	read(inputFile, &first, 1);
	int c = 0;
	while (read(inputFile, &c, 1)) {
		if (c == ' ' && limit > 0) {
			write(outputFile, &first, 1);
			replace_count++;
			limit--;
		} else {
			write(outputFile, &c, 1);
		}
	}

	close(inputFile);
	close(outputFile);

	return replace_count;
}
