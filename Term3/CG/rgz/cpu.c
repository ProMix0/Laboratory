#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]) {
	if (argc < 2) return -1;
	int size = atoi(argv[1]);
	if (size < 1) return -1;

	int size_bytes = size * sizeof(int);
	int* array = malloc(size_bytes);
	arc4random_buf(array, size_bytes);

	for(int i=0;i<size;i++)
		array[i] *= 2;
}
