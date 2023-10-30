#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double calculate_determinant(double** matrix, int size) {
	for (int row = 0; row < size - 1; row++) {
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
		for (int i = row + 1; i < size; i++) {
			double factor = matrix[i][row] / matrix[row][row];
			for (int j = row; j < size; j++) {
				matrix[i][j] -= factor * matrix[row][j];
			}
		}
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
	for (int size = from; size < to; size++) {
		double** matrix =
			malloc(size * size * sizeof(double) + size * sizeof(double*));
		for (int i = 0; i < size; i++)
			matrix[i] = (double*)(matrix + size) + i * size;
		arc4random_buf(matrix + size, size * size * sizeof(double));

		clock_t start = clock();

		double determ = calculate_determinant(matrix, size);

		clock_t end = clock();

		double time = (double)(end - start) / CLOCKS_PER_SEC;

		printf("Elapsed time: %.6lf, result: %lf\n", time, determ);
	}
}