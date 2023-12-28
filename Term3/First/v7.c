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
	int limit = atoi(argv[2]);

	int inputFile = open(fileName, O_RDONLY);
	if (inputFile < 0) {
		printf("Fail to open file\n");
		return -1;
	}

	char destinationFileName[strlen(fileName) + 5];
	strcpy(destinationFileName, fileName);
	int outputFile = creat(strcat(destinationFileName, ".out"), O_WRONLY | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (outputFile < 0) {
		printf("Fail to write to %s\n", destinationFileName);
		return -1;
	}

	int deletions = 0;
	int c = 0;
	while (read(inputFile, &c, 1)) {
		if (c == ' ' && limit > 0) {
			deletions++;
			limit--;
		} else {
			write(outputFile, &c, 1);
		}
	}

	close(inputFile);
	close(outputFile);

	return deletions;
}
