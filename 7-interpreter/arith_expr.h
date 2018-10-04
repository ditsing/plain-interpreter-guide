#ifndef ARITH_EXPR_H
#define ARITH_EXPR_H
#include <stdexcept>

class Program;

class Expr {
  static const double esp;

  public:
  enum class Type {
    Int,
    Double,
  };

  Type type() const {
    return this->type_;
  }

  // Helper method, promote the underlying value to double if necessary.
  // Note if we have more types, this function might look quite different.
  double evaluate_and_promote_to_double() const {
    if (type() == Type::Int) {
      return evaluate_as_int() + 0.0;
    }
    return evaluate_as_double();
  }

  double evaluate_to_double() const {
    if (type() != Type::Double) {
      throw std::logic_error("Evaluating non-double value as double.");
    }
    return evaluate_as_double();
  }

  int evaluate_to_int() const {
    if (type() != Type::Int) {
      throw std::logic_error("Evaluating non-int value as int.");
    }
    return evaluate_as_int();
  }

  virtual ~Expr() {}

  protected:
  // Helper method, assumes the underlying value is an int.
  virtual int evaluate_as_int() const = 0;
  // Helper method, assumes the underlying value is a double.
  virtual double evaluate_as_double() const = 0;

  const Type type_;
  Expr(Type type) : type_(type) {}

  static bool is_zero(double value);

  friend class AddExpr;
  friend class SubExpr;
  friend class MulExpr;
  friend class DivExpr;
  friend class PowExpr;
  friend class WavExpr;
};

Expr* parse_arith_expr(token (*lexer)(), Program *p);

#endif
