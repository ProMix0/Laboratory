#include <unistd.h>

int replaceLimited(int inputFile, int outputFile, int limit) {
	int replace_count = 0;
	int first = 0;
	if (read(inputFile, &first, 1)) write(outputFile, &first, 1);
	int c = 0;
	while (read(inputFile, &c, 1)) {
		if (c == ' ' && limit > 0) {
			write(outputFile, &first, 1);
			replace_count++;
			limit--;
		} else {
			write(outputFile, &c, 1);
		}
	}

	return replace_count;
}