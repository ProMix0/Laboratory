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

	int inputFile = open(fileName, O_RDONLY);
	if (inputFile < 0) {
		printf("Fail to open file\n");
		return -1;
	}

	char destinationFileName[strlen(fileName) + 5];
	strcpy(destinationFileName, fileName);
	int outputFile = creat(strcat(destinationFileName, ".out"), O_WRONLY);
	if (outputFile < 0) {
		printf("Fail to write to %s\n", destinationFileName);
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