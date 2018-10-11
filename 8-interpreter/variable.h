#ifndef VARIABLE_H
#define VARIABLE_H
#include <stdexcept>

class Variable {
  Expr::Type type_;
  bool initialized;
  union {
    int int_val_;
    double double_val_;
  };
  public:
  Variable(Expr::Type type) : type_(type), initialized(false) {}

  Expr::Type type() const {
    return type_;
  }

  void assign(double value) {
    if (type() != Expr::Type::Double) {
      throw std::logic_error("Assigning double value to non-double variable.");
    }
    double_val_ = value;
  }

  void assign(int value) {
    if (type() != Expr::Type::Int) {
      throw std::logic_error("Assigning int value to non-int variable.");
    }
    int_val_ = value;
  }

  void reset() {
    initialized = true;
  }

  int int_val() const {
    if (!initialized) {
      throw std::logic_error("Evaluating uninitialized variable.");
    }
    if (type() != Expr::Type::Int) {
      throw std::logic_error("Referencing non-int value as int.");
    }
    return int_val_;
  }

  double double_val() const {
    if (!initialized) {
      throw std::logic_error("Evaluating uninitialized variable.");
    }
    if (type() != Expr::Type::Double) {
      throw std::logic_error("Referencing non-double value as double.");
    }
    return double_val_;
  }

  // Asking the C++ compiler to disable the following functions, so that we
  // don't accidentally break our code.
  //
  // Copy constructor and copy assign operator.
  // So that `Variable b(1); Variable a(b); a = b;` does not compile.
  Variable(const Variable &v) = delete;
  Variable& operator=(const Variable &v) = delete;

  // Move constructor and move assign operator.
  // So that `Variable b(1); Variable a = std::move(b); return a;` does not
  // compile.
  Variable(Variable &&v) = delete;
  Variable& operator=(Variable &&v) = delete;
};

#endif
