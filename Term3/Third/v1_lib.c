#include <unistd.h>

int deleteChar(int inputFile, int outputFile, char toDelete) {
	int deletions = 0;
	int c = 0;
	while (read(inputFile, &c, 1)) {
		if (c != toDelete) {
			write(outputFile, &c, 1);
		} else {
			deletions++;
		}
	}

	return deletions;
}