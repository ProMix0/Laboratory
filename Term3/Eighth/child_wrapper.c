#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

int child_main(int argc, char* argv[]);

int main(int argc, char* argv[]) {
	if (argc <= 1 || argc % 2 == 0) {
		printf("Arguments not specified or not complete\n");
		return -1;
	}

	int count = argc / 2;
	char** args = argv + 1;
	struct sockaddr_in address, client_address;
	int addr_len = sizeof(address);
	memset(&address, 0, sizeof(address));
	memset(&client_address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(8448);
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	int sock;
	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		printf("Fail to create socket\n");
		return -1;
	}

	if (bind(sock, &address, sizeof(address))) {
		printf("Fail to bind socket\n");
		return -1;
	}

	for (int i = 0; i < count; i++) {
		int pid = fork();
		switch (pid) {
			case 0:
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

				char connect[] = "connect";
				write(sock, connect, sizeof(connect));

				dup2(sock, STDIN_FILENO);
				dup2(sock, STDOUT_FILENO);
				int ret = child_main(1, argv);
				close(sock);
				return ret;
			case -1:
				printf("Fail to fork\n");
				return -1;
			default:
				char buf[64];
				int len =
					recvfrom(sock, buf, 63, 0, &client_address, &addr_len);

				len = snprintf(buf, 64, "%s %s\n\0", *(args), *(args + 1));
				args += 2;

				sendto(sock, buf, len, 0, &client_address, addr_len);

				len = read(sock, buf, 63);
				buf[len] = 0;

				printf("Child done %d changes\n", atoi(buf));
				break;
		}
	}

	close(sock);

	return 0;
}