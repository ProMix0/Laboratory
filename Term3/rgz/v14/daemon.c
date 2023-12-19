#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int process_args(char *arg);
void do_cleanup(int sig);

int sock;

int main() {
	struct sockaddr_in address, client_address;
	unsigned int addr_len = sizeof(address);
	memset(&address, 0, sizeof(address));
	memset(&client_address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(8448);
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		fprintf(stderr, "Fail to create socket\n");
		return -1;
	}

	if (bind(sock, &address, sizeof(address))) {
		fprintf(stderr, "Fail to bind socket\n");
		return -1;
	}

	signal(SIGTERM, do_cleanup);

	while (1) {
		fprintf(stderr, "Waiting for client connection\n");
		char buf[1024];

		int len =
			recvfrom(sock, buf, sizeof(buf), 0, &client_address, &addr_len);

		fprintf(stderr, "Arguments received: \"%s\"\nStart processing\n", buf);
		int result = process_args(buf);
		fprintf(stderr, "Result: %d\n", result);

		fprintf(stderr, "Sending result\n");
		sendto(sock, &result, sizeof(int), 0, &client_address, addr_len);
	}
}

void do_cleanup(int sig) {
	fprintf(stderr, "Stopping service\n");
	close(sock);
	exit(0);
}

int process_args(char *arg) {
	char fileName[256];
	char toReplace;

	if (sscanf(arg, "%s %c", fileName, &toReplace) != 2) {
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

	int replaces = 0;
	int c = 0;
	while (read(inputFile, &c, 1)) {
		if (c >= '0' && c <= '9') {
			write(outputFile, &toReplace, 1);
			replaces++;
		} else {
			write(outputFile, &c, 1);
		}
	}

	close(inputFile);
	close(outputFile);

	return replaces;
}
