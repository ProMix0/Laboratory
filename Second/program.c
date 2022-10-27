#include <stdio.h>
#include <stdlib.h>

int inp_str(char *str, int maxlen);
void sort(char *strings[], int count, int *swaps, int *digitsCount);
int digits_count(char *str);
void out_str(char *str, int length, int number);

void main() {
  char *strings[8];

  for (int i = 0; i < 8; i++) {
    strings[i] = malloc(64);
    inp_str(strings[i], 64);
  }

  int swaps, digitsCount;
  sort(strings, 8, &swaps, &digitsCount);

  printf("Swaps: %d, digits count: %d\n", swaps, digitsCount);
  for (int i = 0; i < 8; i++)
    out_str(strings[i], 0, 0);
}

int inp_str(char *str, int maxlen) {
  int ch, i;
  for (i = 0; i < maxlen && (ch = getchar()) != '\n'; i++)
    str[i] = ch;
  str[i] = 0;

  return 0;
}

void sort(char *strings[], int count, int *swaps, int *digitsCount) {
  *digitsCount = 0;
  for (int i = 0; i < count; i++)
    *digitsCount += digits_count(strings[i]);

  char swap = 1;
  while (swap) {
    swap = 0;

    for (int i = 0; i < count - 1; i++) {
      if (digits_count(strings[i]) < digits_count(strings[i + 1])) {
        char *temp = strings[i];
        strings[i] = strings[i + 1];
        strings[i + 1] = temp;

        swap = 1;
        (*swaps)++;
      }
    }
  }
}

int digits_count(char *str) {
  int count = 0;
  while (*str != 0) {
    if (*str >= '0' && *str <= '9')
      count++;
    str++;
  }
  return count;
}

void out_str(char *str, int length, int number) { printf("%s\n", str); }