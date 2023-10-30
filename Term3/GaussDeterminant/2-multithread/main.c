#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CPUS 2

int row, size;
double** matrix;

void sub_row(int offset) {
	for (int i = row + 1 + offset; i < size; i += CPUS) {
		double factor = matrix[i][row] / matrix[row][row];
		for (int j = row; j < size; j++) {
			matrix[i][j] -= factor * matrix[row][j];
		}
	}
}

void* worker(void* arg) {
	int offset = arg;
	sub_row(offset);
}

double calculate_determinant(double** matrix) {
	for (row = 0; row < size - 1; row++) {
		if (matrix[row][row] != matrix[row][row] ||
			fabs(matrix[row][row]) < 1.0e-11) {
			int pivot_row = row;
			do {
				pivot_row++;
				if (pivot_row == size) return 0;

			} while (matrix[pivot_row][row] != matrix[pivot_row][row] ||
					 fabs(matrix[pivot_row][row]) < 1.0e-11);
			double* swap = matrix[row];
			matrix[row] = matrix[pivot_row];
			matrix[pivot_row] = swap;
		}
		pthread_t threads[CPUS - 1];
		for (int i = 1; i < CPUS; i++)
			pthread_create(threads + i - 1, 0, worker, i);
		sub_row(0);
		for (int i = 1; i < CPUS; i++) pthread_join(threads[i - 1], 0);
	}

	double result = 1;
	for (int i = 0; i < size; i++) {
		result *= matrix[i][i];
	}
	return result;
}

int main(int argc, char* argv[]) {
	int from, to;
	switch (argc) {
		// Do one run
		case 2:
			from = atoi(argv[1]);
			to = from + 1;
			break;
		// Run from - to
		case 3:
			from = atoi(argv[1]);
			to = atoi(argv[2]);
			break;
		default:
			fprintf(stderr, "Undefined action\n");
			return 1;
	}
	if (from <= 0) {
		fprintf(stderr, "Invalid start size\n");
		return 1;
	}
	for (size = from; size < to; size++) {
		matrix = malloc(size * size * sizeof(double) + size * sizeof(double*));
		for (int i = 0; i < size; i++)
			matrix[i] = (double*)(matrix + size) + i * size;
		arc4random_buf(matrix + size, size * size * sizeof(double));

		struct timespec start, end;

		clock_gettime(CLOCK_REALTIME, &start);

		double determ = calculate_determinant(matrix);

		clock_gettime(CLOCK_REALTIME, &end);

		free(matrix);

		double time =
			(end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1.0e9;

		printf("Elapsed time: %.6lf, result: %lf\n", time, determ);
	}
}