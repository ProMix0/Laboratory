#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <mpi.h>

int seq_len(int64_t n);

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: ./serial <N> <len>\n");
    return -1;
  }

  int64_t n_max = atoll(argv[1]);
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
  MPI_Init(0, 0);

  int instances, myid;
  MPI_Comm_size(MPI_COMM_WORLD, &instances);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);

  int64_t result_n = -1;
  int result_len = 0;
  for (int64_t i = n_max - 1 - myid; i > 0; i -= instances) {
    int len = seq_len(i);
    if (len == target_len) {
      result_n = i;
      result_len = len;
      break;
    }
  }

  int64_t reduced_n = -1;
  MPI_Reduce(&result_n, &reduced_n, 1, MPI_INT64_T, MPI_MAX, 0, MPI_COMM_WORLD);

  MPI_Finalize();

  if (myid == 0) {
    gettimeofday(&end, 0);
    double time =
        (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    printf("Time: %lf\n", time);

    if (reduced_n == -1) {
      printf("Can't find N that matches conditions\n");
      return 1;
    }

    printf("N = %lu\n", reduced_n);
  }

  return 0;
}

int seq_len(int64_t n) {
  int len = 1;
  while (n > 1) {
    n = n % 2 ? (3 * n + 1) : (n / 2);
    len++;
  }
  return len;
}
