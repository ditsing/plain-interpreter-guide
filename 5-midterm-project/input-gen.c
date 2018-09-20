#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define N 100
#define L 30
#define N_CHS 7

const char *chs = "12+*()~";
bool selected[N_CHS];
char buffer[L];

void gen_input(int p) {
  if (p == N_CHS) {
    buffer[p] = '\0';
    printf("%s\n", buffer);
    return;
  }

  for (int i = 0; i < N_CHS; i++) {
    if (!selected[i]) {
      selected[i]  = true;
      buffer[p] = chs[i];
      gen_input(p+1);
      selected[i] = false;
    }
  }
}

int main() {
  memset(selected, 0, sizeof(selected));
  gen_input(0);
  return 0;
}
