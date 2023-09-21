#include <stdio.h>
#include <unistd.h>

int replaceChar(int inputFile, int outputFile, char toReplace) {
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

	return replaces;
}