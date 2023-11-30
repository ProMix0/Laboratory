#include <dlfcn.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int child_main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("Wrong arguments count\n");
		return -1;
	}

	char *fileName = argv[1];
	char toReplace = *argv[2];

	int inputFile = open(fileName, O_RDONLY);
	if (inputFile < 0) {
		printf("Fail to open file\n");
		return -1;
	}

	char destinationFileName[strlen(fileName) + 5];
	strcpy(destinationFileName, fileName);
	int outputFile = creat(strcat(destinationFileName, ".out"), O_WRONLY|S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (outputFile < 0) {
		printf("Fail to write to %s\n", destinationFileName);
		return -1;
	}

	int result = 0;
	void *handle;
	handle = dlopen("./libv14.so", RTLD_LOCAL | RTLD_NOW);
	if (handle) {
		int (*func)(int, int, char);
		*(void **)(&func) = dlsym(handle, "replaceChar");
		if (func) {
			result = (*func)(inputFile, outputFile, toReplace);
		} else {
			printf("Failed to find function\n");
			return -1;
		}
		dlclose(handle);
	} else {
		printf("Failed to load library\n");
		return -1;
	}

	close(inputFile);
	close(outputFile);

	return result;
}
