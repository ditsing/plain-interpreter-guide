#ifndef ARITH_EXPR_H
#define ARITH_EXPR_H

class Expr {
  static const double esp;
  public:
  virtual double evaluate() const = 0;

  protected:
  static bool is_zero(double value);
};

Expr* parse_arith_expr(token (*lexer)());

#endif
