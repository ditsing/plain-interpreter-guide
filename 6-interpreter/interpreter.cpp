#include <cstdio>
#include <string>
#include "lexer.h"
#include "arith_expr.h"
#include "interpreter.h"

using std::string;

int main() {
  Expr *e;
  while ((e = parse_arith_expr(lexer))) {
    printf("%lf\n", e->evaluate());
  }
}
