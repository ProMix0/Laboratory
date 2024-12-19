#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int compact_subseq(uint64_t n);

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: ./serial <N>\n");
    return -1;
  }

  uint64_t n = atoll(argv[1]);

  if (n < 10) {
    printf("N is too small: %lu\n", n);
    return -1;
  }

  struct timeval start, end;
  gettimeofday(&start, 0);
  uint64_t result = -1;

  uint64_t M = 0, m, p;
  do {
    n++;
    int seq_sum = compact_subseq(n);
    m = 2;
    p = 2;
    while (1) {
      M = pow(m, p);
      if (M < seq_sum)
        p++;
      else if (M > seq_sum) {
        p = 2;
        m++;
        if (pow(m, p) > seq_sum) {
          M = 0;
          break;
        }
      } else
        break;
    }
  } while (!M);

  gettimeofday(&end, 0);

  double time =
      (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
  printf("Time: %lf\n", time);

  printf("m: %ld, p: %ld, M: %ld\n", m, p, n);

  return 0;
}

int compact_subseq(uint64_t n) {
  uint64_t sum = 0;
  int tmp = n / 10;
  int power = 1;
  for (; tmp; tmp = tmp / 10) {
    sum += tmp;
    power *= 10;
  }
  n = n % power;
  tmp = n;
  while (tmp) {
    int power = 1;
    for (; tmp; tmp = tmp / 10) {
      sum += tmp;
      power *= 10;
    }
    power /= 10;
    n = n % power;
    tmp = n;
  }
  return sum;
}
