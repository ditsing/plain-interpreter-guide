#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define N 100
#define L 30
#define N_OPS 5

const char ops[] = "+-*/^";

int main() {
  // No srand, on purpose.
  for (int i = 0; i < N; i++) {
    bool is_power = false;
    for (int j = 0; j < L; j++) {
      if (!is_power) {
        printf("%d", rand() % N);
      } else {
        printf("%d", rand() % 4);
      }
      if (j != L-1) {
        char c = ops[rand() % N_OPS];
        printf("%c", c);
        is_power = c == '^';
      }
    }
    printf("\n");
  }

  return 0;
}
