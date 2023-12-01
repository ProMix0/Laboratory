#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
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
	int pids[count];
	int shm;
	sem_t* sem = sem_open("/nstu", O_CREAT, 0777, count);
	if ((shm = shm_open("lab6shm", O_CREAT | O_RDWR, 0777)) == -1) {
		printf("Fail to open shared memory\n");
		return -1;
	}
	if (ftruncate(shm, count * sizeof(int))) {
		printf("Fail to set shared memory size\n");
		return -1;
	}
	results = mmap(0, count * sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED,
				   shm, 0);
	args = argv;

	for (int i = 0; i < count; i++) {
		int pid = fork();
		switch (pid) {
			case 0:
				if ((shm = shm_open("lab6shm", O_RDWR, 0777)) == -1 ||
					ftruncate(shm, count * sizeof(int))) {
					printf("Fail to open shared memory\n");
					return -1;
				}
				results = mmap(0, count * sizeof(int), PROT_WRITE | PROT_READ,
							   MAP_SHARED, shm, 0);
				sem_wait(sem);
				return child_main(i);
			case -1:
				printf("Fail to fork\n");
				return -1;
			default:
				pids[i] = pid;
				break;
		}
	}
	int summ = 0;
	for (int i = 0; i < count; i++) {
		int status = 0;
		int err = waitpid(pids[i], &status, 0);
		if (status < 0) {
			printf("Child return error: %d\n", results[i]);
		} else {
			printf("Child done %d changes\n", results[i]);
			summ = summ + results[i];
		}
	}
	printf("%d", summ);

	munmap(results, 4096);
	shm_unlink("lab6shm");
	sem_unlink("/nstu");
	return 0;
}
