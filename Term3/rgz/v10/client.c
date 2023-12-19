#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>

#include "common.h"

int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("Wrong arguments count\n");
		return -1;
	}

	char *fileName = argv[1];
	int limit = atoi(argv[2]);
	if (limit < 0) {
		printf("Invalid limit; must not be negative\n");
		return -1;
	}

	int sh_file = open(SH_FILE, O_RDWR);
	if (sh_file < 0) {
		printf("Unable to open shared file: \"" SH_FILE "\"\n");
		return -1;
	}
	char *map_mem =
		mmap(0, 1024, PROT_WRITE | PROT_READ, MAP_SHARED, sh_file, 0);
	if (map_mem == MAP_FAILED) {
		printf("Unable to map shared file\n");
		return -1;
	}

	int queue = msgget(MSG_QUEUE, 0666);
	if (queue < 0) {
		printf("Unable to open message queue\n");
		return -1;
	}
	message_t message;

	message.type = WRITE_REQUEST;
	msgsnd(queue, &message, 0, 0);

	char cwd[256];
	getcwd(cwd, sizeof(cwd));
	snprintf(map_mem, 1024, "%s/%s %d", cwd, fileName, limit);

	message.type = WRITE_ACK;
	msgsnd(queue, &message, 0, 0);
	msgrcv(queue, 0, 0, READ_RESULT, 0);

	int result = *((int *)map_mem);
	message.type = READ_DONE;
	msgsnd(queue, &message, 0, 0);

	printf("Result: %d\n", result);
	return 0;
}
