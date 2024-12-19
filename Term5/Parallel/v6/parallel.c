#include <math.h>
#include <omp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int compact_subseq(uint64_t n);

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: ./parallel <N>\n");
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

  uint64_t M = 0, m = 0, p = 0;
  int exit = 0;

#pragma omp parallel shared(n, exit, M, m, p)
  {
    uint64_t local_n = n - omp_get_thread_num();
    uint64_t local_M = 0;
    int local_m, local_p;
    int local_exit = 0;

#pragma omp flush(exit)
    while (!exit) {
      local_n += omp_get_num_threads();
      int seq_sum = compact_subseq(local_n);
      local_m = 2;
      local_p = 2;
      local_M = 0;

      while (1) {
        local_M = pow(local_m, local_p);
        if (local_M < seq_sum)
          local_p++;
        else if (local_M > seq_sum) {
          local_p = 2;
          local_m++;
          if (pow(local_m, local_p) > seq_sum) {
            local_M = 0;
            break;
          }
        } else {
          local_exit = 1;
          break;
        }
      }

      if (local_exit) {
#pragma omp critical
        {
          if (!exit) {
            M = local_M;
            m = local_m;
            p = local_p;
            n = local_n;
            exit = 1;
          }
        }
      }

#pragma omp flush(exit)
    }
  }

  gettimeofday(&end, 0);

  double time =
      (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
  printf("Time: %lf\n", time);

  printf("m: %ld, p: %ld, M: %lu\n", m, p, n);

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
