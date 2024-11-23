#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int N;

struct monominal {
  int multiplier;
  int powers[];
};

struct polynominal {
  int count;
};

struct monominal_matrix {
  int rows, cols;
};

struct polynominal *realloc_polynom(struct polynominal *polynom, int size);
struct polynominal *read_polynom(FILE *f);
struct monominal_matrix *alloc_monominal_matrix(int rows, int cols);
void fill_matrix(struct monominal_matrix *matrix,
                 struct polynominal *first_poly,
                 struct polynominal *second_poly);
void produce(struct monominal *result, struct monominal *first,
             struct monominal *second);
int powers_matches(struct monominal *first, struct monominal *second);
void sum_up(struct monominal_matrix *matrix);

static inline struct monominal *get_monominal_from_plain(void *data,
                                                         int index) {
  return (struct monominal *)((int *)data + (index) * (N + 1));
}

static inline struct monominal *get_monominal(struct polynominal *polynom,
                                              int index) {
  return get_monominal_from_plain(polynom + 1, index);
}

static inline struct monominal *
get_monominal_from_matrix(struct monominal_matrix *matrix, int row, int col) {
  return get_monominal_from_plain(matrix + 1, row * matrix->cols + col);
}

int main(int argc, char *argv[]) {
  if (argc < 4) {
    printf("Usage: ./serial <N> <file1> <file2>\n");
    return -1;
  }

  N = atoi(argv[1]);

  if (N < 1) {
    printf("N is too small: %d\n", N);
    return -1;
  }

  FILE *file1 = fopen(argv[2], "r");
  if (!file1) {
    printf("File '%s' (second argument) not exists!\n", argv[2]);
    return 1;
  }

  FILE *file2 = fopen(argv[3], "r");
  if (!file2) {
    printf("File '%s' (third argument) not exists!\n", argv[3]);
    fclose(file1);
    return 1;
  }

  struct polynominal *polynom1 = read_polynom(file1);
  struct polynominal *polynom2 = read_polynom(file2);

  if (!polynom1) {
    printf("Unable to read first polynominal\n");
    return 1;
  }
  if (!polynom2) {
    printf("Unable to read second polynominal\n");
    return 1;
  }

  /*
    printf("First\n");
    for (int i = 0; i < polynom1->count; i++) {
      printf("%d ", get_monominal(polynom1, i)->multiplier);
      for (int j = 0; j < N; j++) {
        printf("%d ", get_monominal(polynom1, i)->powers[j]);
      }
      printf("\n");
    }
    printf("Second\n");
    for (int i = 0; i < polynom2->count; i++) {
      printf("%d ", get_monominal(polynom2, i)->multiplier);
      for (int j = 0; j < N; j++) {
        printf("%d ", get_monominal(polynom2, i)->powers[j]);
      }
      printf("\n");
    }
    */

  struct monominal_matrix *matrix =
      alloc_monominal_matrix(polynom1->count, polynom2->count);

  struct timeval start, end;
  gettimeofday(&start, 0);

  fill_matrix(matrix, polynom1, polynom2);

  sum_up(matrix);

  gettimeofday(&end, 0);

  int i, j, k;
  for (i = 0; i < matrix->rows; i++) {
    for (j = 0; j < matrix->cols; j++) {
      struct monominal *to_print = get_monominal_from_matrix(matrix, i, j);
      if (!to_print->multiplier)
        continue;
      fprintf(stderr, "%d ", to_print->multiplier);
      for (k = 0; k < N; k++) {
        fprintf(stderr, "%d ", to_print->powers[k]);
      }
      fprintf(stderr, "\n");
    }
  }

  double time =
      (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
  printf("Time: %lf\n", time);

  return 0;
}

struct polynominal *read_polynom(FILE *f) {
  int size = 64;
  struct polynominal *result = realloc_polynom(0, size);
  int int_index = 0;
  int tmp;
  int readed = 0;
  while (fscanf(f, "%d", &tmp) == 1) {
    readed++;
    ((int *)(result + 1))[int_index++] = tmp;
    if (readed / (N + 1) == size) {
      size *= 2;
      result = realloc_polynom(result, size);
    }
  }
  fclose(f);
  if (!readed || readed % (N + 1)) {
    printf("Invalid input file\n");
    free(result);
    return 0;
  }
  result->count = readed / (N + 1);
  return result;
}

void fill_matrix(struct monominal_matrix *matrix,
                 struct polynominal *first_poly,
                 struct polynominal *second_poly) {
  int i, j;
  for (i = 0; i < matrix->rows; i++) {
    for (j = 0; j < matrix->cols; j++) {
      produce(get_monominal_from_matrix(matrix, i, j),
              get_monominal(first_poly, i), get_monominal(second_poly, j));
    }
  }
}

void produce(struct monominal *result, struct monominal *first,
             struct monominal *second) {
  result->multiplier = first->multiplier * second->multiplier;
  int i;
  for (i = 0; i < N; i++)
    result->powers[i] = first->powers[i] + second->powers[i];
}

struct polynominal *realloc_polynom(struct polynominal *polynom, int size) {
  return realloc(polynom,
                 sizeof(struct polynominal) +
                     size * (sizeof(struct monominal) + N * sizeof(int)));
}

struct monominal_matrix *alloc_monominal_matrix(int rows, int cols) {
  struct monominal_matrix *matrix =
      malloc(sizeof(struct monominal_matrix) +
             rows * cols * (sizeof(struct monominal) + N * sizeof(int)));
  matrix->rows = rows;
  matrix->cols = cols;
  return matrix;
}

void sum_up(struct monominal_matrix *matrix) {
  int i, j;
  for (i = 0; i < matrix->rows * matrix->cols; i++) {
    struct monominal *source = get_monominal_from_plain(matrix + 1, i);
    for (j = i + 1; j < matrix->rows * matrix->cols; j++) {
      struct monominal *target = get_monominal_from_plain(matrix + 1, j);
      if (powers_matches(source, target)) {
        target->multiplier += source->multiplier;
        source->multiplier = 0;
	break;
      }
    }
  }
}

int powers_matches(struct monominal *first, struct monominal *second) {
 int i;
  for (i = 0; i < N; i++)
    if (first->powers[i] - second->powers[i])
      return 0;
  return 1;
}
