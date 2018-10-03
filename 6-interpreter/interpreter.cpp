#include <cstdio>
#include "lexer.h"
#include "arith_expr.h"
#include "interpreter.h"

int main() {
  Expr *e;
  while ((e = parse_arith_expr(lexer)) != nullptr) {
    try {
      printf("%lf\n", e->evaluate());
    } catch (const RuntimeError &e) {
      printf("Runtime error: %s\n", e.what());
    }
  }
}
