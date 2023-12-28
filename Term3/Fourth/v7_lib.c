#include <unistd.h>

int replaceLimited(int inputFile, int outputFile, int limit) {
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
	return deletions;
}
