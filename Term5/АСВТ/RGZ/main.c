#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int32_t booth_algorithm_rewrite(int16_t m, int16_t r) {
  int bit_length = sizeof(m) * 8;

  r <<= 1;
  uint32_t result = 0;

  for (int i = 0; i < bit_length; i++) {
    switch (r & 3) {
    case 0b01:
      result += m;
      break;
    case 0b10:
      result -= m;
      break;
    }
    m <<= 1;
    r >>= 1;
  }

  return (int32_t)(uint32_t)result;
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: %s N1 N2\n", argv[0]);
    return -1;
  }

  int16_t m = (int16_t)atoi(argv[1]);
  int16_t r = (int16_t)atoi(argv[2]);
  printf("Readed args: %d %d\n", m, r);

  int32_t result = booth_algorithm_rewrite(m, r);

  printf("Result: %d\n", result);
  return 0;
}
