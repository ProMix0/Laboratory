#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("Wrong arguments count\n");
		return -1;
	}

	char *fileName = argv[1];
	char toReplace = *argv[2];

	struct sockaddr_in address;
	int addr_len = sizeof(address);
	int sock;
	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		printf("Fail to create socket\n");
		return -1;
	}

	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(8448);

	if (inet_aton("127.0.0.1", &address.sin_addr) == 0) {
		printf("Fail to set addrress\n");
		return -1;
	}

	if (connect(sock, &address, sizeof(address))) {
		printf("Fail to connect\n");
		return -1;
	}

	char cwd[256], buf[1024];
	getcwd(cwd, sizeof(cwd));
	int len = snprintf(buf, 1024, "%s/%s %c", cwd, fileName, toReplace);

	write(sock, buf, len + 1);

	int result;
	read(sock, &result, sizeof(int));

	printf("Result: %d\n", result);
	close(sock);
	return 0;
}
