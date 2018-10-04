/**
 * An advanced calculator that takes in a line and evalucate it as an math
 * expression.
 */
#include <cstdio>
#include <cctype>
#include <cmath>
#include <vector>
#include <stdexcept>
#include "lexer.h"
#include "arith_expr.h"
#include "interpreter.h"

using std::vector;
typedef Expr::Type Type;

bool Expr::is_zero(double value) {
  return value < esp and value > -esp;
}

const double Expr::esp = std::numeric_limits<double>::epsilon();

class Num: public Expr {
  double value;

  public:
  Num(double value) : value(value), Expr(Type::Double) {}

  protected:
  int evaluate_as_int() const override {
    throw RuntimeError("Cannot evaluate num as int.");
  }

  double evaluate_as_double() const override {
    return value;
  }
};

Type choose_type(Expr *left, Expr *right) {
  if (left->type() == Type::Int
      && right->type() == Type::Int) {
    return Type::Int;
  }
  return Type::Double;
}

class AddExpr: public Expr {
  const Expr* left;
  const Expr* right;

  public:
  AddExpr(Expr* left, Expr* right) : left(left), right(right),
                                     Expr(choose_type(left, right)) {}

  protected:
  int evaluate_as_int() const override {
    return left->evaluate_as_int() + right->evaluate_as_int();
  }

  double evaluate_as_double() const override {
    return left->evaluate_and_promote_to_double() +
      right->evaluate_and_promote_to_double();
  }
};

class SubExpr: public Expr {
  const Expr* left;
  const Expr* right;

  public:
  SubExpr(Expr* left, Expr* right) : left(left), right(right),
                                     Expr(choose_type(left, right)) {}

  protected:
  int evaluate_as_int() const override {
    return left->evaluate_as_int() - right->evaluate_as_int();
  }

  double evaluate_as_double() const override {
    return left->evaluate_and_promote_to_double() -
      right->evaluate_and_promote_to_double();
  }
};

class MulExpr: public Expr {
  const Expr* left;
  const Expr* right;

  public:
  MulExpr(Expr* left, Expr* right) : left(left), right(right),
                                     Expr(choose_type(left, right)) {}

  protected:
  int evaluate_as_int() const override {
    int left_value = left->evaluate_as_int();
    if (left_value == 0) {
      return 0;
    }
    return left_value * right->evaluate_as_int();
  }

  double evaluate_as_double() const override {
    double left_value = left->evaluate_and_promote_to_double();
    if (is_zero(left_value)) {
      return 0.0;
    }
    return left_value * right->evaluate_and_promote_to_double();
  }
};

class DivExpr: public Expr {
  const Expr* left;
  const Expr* right;

  public:
  DivExpr(Expr* left, Expr* right) : left(left), right(right),
                                     Expr(choose_type(left, right)) {}

  protected:
  int evaluate_as_int() const override {
    int left_value = left->evaluate_as_int();
    int right_value = right->evaluate_as_int();
    if (right_value == 0) {
      throw RuntimeError("Divded by zero");
    }
    return left_value / right_value;
  }

  double evaluate_as_double() const override {
    double left_value = left->evaluate_and_promote_to_double();
    double right_value = right->evaluate_and_promote_to_double();
    if (is_zero(right_value)) {
      throw RuntimeError("Divded by zero");
    }
    return left_value / right_value;
  }
};

/**
 * HOMEWORK 1: Power and wave operator
 *
 * Complete the protected methods for `PowExpr` and `WavExpr`. Note that power
 * operator always returns a double, while wave operator always returns an int.
 * Thus a `PowExpr` always has `Expr::Type::Double` and a `WavExpr` always has
 * `Expr::Type::Int`.
 */
class PowExpr: public Expr {
  const Expr* left;
  const Expr* right;

  public:
  PowExpr(Expr* left, Expr* right) : left(left), right(right),
                                     Expr(Type::Double) {}
  protected:
  int evaluate_as_int() const override {
  }

  double evaluate_as_double() const override {
  }
};

class WavExpr: public Expr {
  Expr* expr;
  public:
  WavExpr(Expr* expr) : expr(expr), Expr(Type::Int) {}

  protected:
  int evaluate_as_int() const override {
  }

  double evaluate_as_double() const override {
  }
};

vector<char> op_stack;
vector<Expr*> num_stack;

/**
 * Process the last operator in the stack.
 */
void process_last_operator() {
  // Special case for unary operators.
  if (op_stack.back() == '~') {
    op_stack.pop_back();
    Expr *operand = num_stack.back();
    num_stack.pop_back();
    num_stack.push_back(new WavExpr(operand));
    return;
  }

  // Takes 2 expressions from stack.
  Expr* right_operand = num_stack.back();
  num_stack.pop_back();
  Expr* left_operand = num_stack.back();
  num_stack.pop_back();

  // Calculation based on the current operator.
  Expr* result = nullptr;
  switch (op_stack.back()) {
    case '+':
      result = new AddExpr(left_operand, right_operand);
      break;
    case '-':
      result = new SubExpr(left_operand, right_operand);
      break;
    case '*':
      result = new MulExpr(left_operand, right_operand);
      break;
    case '/':
      result = new DivExpr(left_operand, right_operand);
      break;
    case '^':
      result = new PowExpr(left_operand, right_operand);
      break;
    default:
      THROW_ERROR_FORMAT(
          std::logic_error,
          "Unexpected operator %c",
          op_stack.back());
  }
  // Remove the processed operator from stack;
  op_stack.pop_back();

  // Replace the processed numbers with the result.
  num_stack.push_back(result);
}

bool is_unary_operator(int type) {
  return type == '~';
}

class stack_releaser {
  public:
  ~stack_releaser() {
    num_stack.clear();
    op_stack.clear();
  }
};

Expr* parse_arith_expr(token (*lexer)(), Program *p) {
  stack_releaser releaser;
  token t;
  bool expecting_number = true;
  while (t = lexer(), t.type != ';' && t.type) {
    if (expecting_number ^
          (t.type == INTEGER_LITERAL || t.type == '('
           || is_unary_operator(t.type))) {
      THROW_ERROR_FORMAT_LINE(
          CompilingError,
          t,
          "Consecutive numbers or operators found: %d",
          t.type);
    }
    expecting_number =
      !(t.type == INTEGER_LITERAL || t.type == ')');

    switch (t.type) {
      case INTEGER_LITERAL:
        num_stack.push_back(new Num(t.int_val));
        break;
      case '(':
      case '~':
        // Do nothing.
        op_stack.push_back(t.ops_val);
        break;
      case '^':
        // Power operator has the highest priority.
        while (!op_stack.empty() &&
            (op_stack.back() == '^' || op_stack.back() == '~')) {
          process_last_operator();
        }
        op_stack.push_back(t.ops_val);
        break;
      case '*':
      case '/':
        // Process all operators that have a higher or equal priority.
        while (!op_stack.empty()
            && (op_stack.back() == '^' || op_stack.back() == '*'
             || op_stack.back() == '/' || op_stack.back() == '~')) {
          process_last_operator();
        }
        op_stack.push_back(t.ops_val);
        break;
      case '+':
      case '-':
        // All other operators have higher priorities.
        while (!op_stack.empty() &&
            (op_stack.back() == '^' || op_stack.back() == '*'
             || op_stack.back() == '/' || op_stack.back() == '+'
               || op_stack.back() == '-' || op_stack.back() == '~')) {
          process_last_operator();
        }
        op_stack.push_back(t.ops_val);
        break;
      case ')':
        while (!op_stack.empty() && op_stack.back() != '(') {
          process_last_operator();
        }
        if (op_stack.empty()) {
          // Error: expecting parenthesis but not getting one.
          throw CompilingError("Unmatched right parenthesis in input");
        }
        // Remove '('
        op_stack.pop_back();
        break;
      default:
        THROW_ERROR_FORMAT_LINE(
            CompilingError,
            t,
            "Unexpected token with type %d",
            t.type);
    }
  }

  if (expecting_number) {
    if (num_stack.empty()) {
      // We had no input.
      // Returning to caller for a better error message.
      return nullptr;
    }
    throw CompilingError("Missing operand");
  }
  while (!op_stack.empty()) {
    if (op_stack.back() == '(') {
      throw CompilingError("Unmatched left parenthesis in input");
    }
    process_last_operator();
  }

  Expr *expr = num_stack.back();
  num_stack.pop_back();
  return expr;
}
