#include <cstdio>
#include "lexer.h"
#include "arith_expr.h"
#include "interpreter.h"

int main() {
  Expr *e;
  while ((e = parse_arith_expr(lexer)) != nullptr) {
    printf("%lf\n", e->evaluate());
  }
}
