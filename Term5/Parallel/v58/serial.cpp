#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

uint32_t *get_array(int count);

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: ./serial <N>\n");
    return -1;
  }

  int count = atoi(argv[1]);
  if (count < 2) {
    printf("Must be at least two elements\n");
    return -1;
  }

  count = (count + 3) & ~3;

  uint32_t *array = get_array(count);

  struct timeval start, end;
  gettimeofday(&start, 0);

  for (int i = 0; i < count - 1; i++) {
    for (int j = 0; j < count - i - 1; j++) {
      if (array[j] > array[j + 1]) {
        uint32_t tmp = array[j];
        array[j] = array[j + 1];
        array[j + 1] = tmp;
      }
    }
  }

  gettimeofday(&end, 0);
  double time_result =
      (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
  printf("Time: %lf\n", time_result);

  FILE *file = fopen("result_serial", "w+");
  if (file) {
    for (int i = 0; i < count; i++)
      fprintf(file, "%d\n", array[i]);
    fclose(file);
  }
  return 0;
}

uint32_t *get_array(int count) {
  if (getenv("SEED"))
    srand(atoi(getenv("SEED")));
  else
    srand((0));
  uint32_t *result = (uint32_t *)malloc(sizeof(uint32_t) * count);
  for (int i = 0; i < count; i++)
    result[i] = rand();
  return result;
}
