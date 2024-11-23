#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if (argc < 4) {
    printf("Usage: ./gendata <N> <count> <seed>\n");
    return 1;
  }

  int N = atoi(argv[1]);
  int count = atoi(argv[2]);
  int seed = atoi(argv[2]);
  int i, j;

  if (N <= 0) {
    printf("N must be positive!\n");
    return 1;
  }

  if (count <= 0) {
    printf("Count must be positive!\n");
    return 1;
  }

  srand(seed);

  for (i = 0; i < count; i++) {
    for (j = 0; j < N + 1; j++) {
      printf("%d ", rand() % 10 - 2);
    }
    printf("\n");
  }

  return 0;
}
