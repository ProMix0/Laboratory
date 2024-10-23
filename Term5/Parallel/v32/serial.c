#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int seq_len(uint64_t n);

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: ./serial <N> <len>\n");
    return -1;
  }

  uint64_t n_max = atoll(argv[1]);
  int target_len = atoi(argv[2]);

  if (n_max < 1) {
    printf("N is too small: %lu\n", n_max);
    return -1;
  }

  if (target_len < 1) {
    printf("len is too small: %d\n", target_len);
    return -1;
  }

  struct timeval start, end;
  gettimeofday(&start, 0);
  uint64_t result_n = -1;

  int result_len = 0;
  for (uint64_t i = n_max - 1; i > 0; i--) {
    int len = seq_len(i);
    if (len == target_len) {
      result_n = i;
      result_len = len;
      break;
    }
  }

  gettimeofday(&end, 0);

  double time =
      (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
  printf("Time: %lf\n", time);
  if (result_n == -1) {
    printf("Can't find N, matching conditions\n");
    return 1;
  }

  printf("Len of N(%lu) is: %d\n", result_n, result_len);

  return 0;
}

int seq_len(uint64_t n) {
  int len = 1;
  while (n > 1) {
    n = n % 2 ? (3 * n + 1) : (n / 2);
    len++;
  }
  return len;
}
