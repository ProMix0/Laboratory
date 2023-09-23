#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int child_main(int i);
char** args;
int* results;

int main(int argc, char* argv[]) {
	if (argc <= 1 || argc % 2 == 0) {
		printf("Arguments not specified or not complete\n");
		return -1;
	}

	int count = argc / 2;
	pthread_t threads[count];
	results = malloc(sizeof(int) * count);
	args = argv;

	for (int i = 0; i < count; i++) {
		pthread_create(threads + i, 0, child_main, i);
	}

	for (int i = 0; i < count; i++) {
		pthread_join(threads[i], 0);
		printf("Child done %d changes\n", results[i]);
	}

	free(results);
	return 0;
}