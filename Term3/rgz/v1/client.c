#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"

int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("Wrong arguments count\n");
		return -1;
	}

	char *fileName = argv[1];
	char toDelete = *argv[2];

	int arg_fd = open(ARG_PIPE, O_WRONLY);
	if (arg_fd < 0) {
		fprintf(stderr, "Unable to open argument pipe\n");
		return -1;
	}
	int res_fd = open(RES_PIPE, O_RDONLY);
	if (res_fd < 0) {
		fprintf(stderr, "Unable to open result pipe\n");
		return -1;
	}

	char cwd[256], buf[1024];
	getcwd(cwd, sizeof(cwd));
	int len = snprintf(buf, 1024, "%s/%s %c", cwd, fileName, toDelete);

	write(arg_fd, buf, len + 1);

	int result;
	read(res_fd, &result, sizeof(int));

	printf("Result: %d\n", result);
	return 0;
}
