#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define N 100
#define L 30
#define N_OPS 6

const char ops[] = "+-*//^";

void gen_expr(int, int);

int gen_num_or_parentheses(int l, int n) {
  int n_l = rand() % (l + l / 2 + 2);
  if (rand() % 10 < 1) {
    printf("~");
  }
  if (n_l > l) {
    printf("(");
    gen_expr(n_l - l, n);
    printf(")");
    return n_l - l;
  } else {
    printf("%d", rand() % n);
    return 1;
  }
}

void gen_expr(int l, int n) {
  l -= gen_num_or_parentheses(l, n);
  while (l > 0) {
    char c = ops[rand() % N_OPS];
    printf("%c", c);
    --l;
    l -= gen_num_or_parentheses(l, c == '^' ? 4 : n);
  }
}

int main() {
  // No srand, on purpose.
  for (int i = 0; i < N; i++) {
    gen_expr(L, N);
    printf("\n");
  }

  return 0;
}
