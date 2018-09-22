/**
 * An advanced calculator that takes in a line and evalucate it as an math
 * expression.
 */
#include <cstdio>
#include <cctype>
#include <cmath>
#include <vector>
#include <stdexcept>
#include <string>
#include "lexer.h"
#include "arith_expr.h"
#include "interpreter.h"

using std::vector;
using std::overflow_error;
using std::range_error;
using std::invalid_argument;
using std::domain_error;
using std::string;

bool Expr::is_zero(double value) {
  return value < esp and value > -esp;
}

const double Expr::esp = std::numeric_limits<double>::epsilon();

class Num: public Expr {
  double value;

  public:
  Num(double value) : value(value) {}
  double evaluate() const override {
    return value;
  }
};

class AddExpr: public Expr {
  const Expr* left;
  const Expr* right;

  public:
  AddExpr(Expr* left, Expr* right) : left(left), right(right) {}
  double evaluate() const override {
    return left->evaluate() + right->evaluate();
  }
};

class SubExpr: public Expr {
  const Expr* left;
  const Expr* right;

  public:
  SubExpr(Expr* left, Expr* right) : left(left), right(right) {}
  double evaluate() const override {
    return left->evaluate() - right->evaluate();
  }
};

class MulExpr: public Expr {
  const Expr* left;
  const Expr* right;

  public:
  MulExpr(Expr* left, Expr* right) : left(left), right(right) {}
  double evaluate() const override {
    double left_value = left->evaluate();
    if (is_zero(left_value)) {
      return 0;
    }
    return left_value * right->evaluate();
  }
};

class DivExpr: public Expr {
  const Expr* left;
  const Expr* right;

  public:
  DivExpr(Expr* left, Expr* right) : left(left), right(right) {}
  double evaluate() const override {
    double left_value = left->evaluate();
    double right_value = right->evaluate();
    if (is_zero(right_value)) {
      throw overflow_error("Divded by zero");
    }
    return left_value / right_value;
  }
};

class PowExpr: public Expr {
  const Expr* left;
  const Expr* right;

  public:
  PowExpr(Expr* left, Expr* right) : left(left), right(right) {}
  double evaluate() const override {
    double left_value = left->evaluate();
    double right_value = right->evaluate();
    if (left_value < 0 && !is_zero(right_value - round(right_value))) {
      throw range_error("Cannot calculate non-integer power of negative value");
    }
    return pow(left->evaluate(), right->evaluate());
  }
};

class WavExpr: public Expr {
  Expr* expr;
  public:
  WavExpr(Expr* expr) : expr(expr) {}
  double evaluate() const override {
    return round(expr->evaluate());
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
    Expr* operand = num_stack.back();
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
      // Should never happen.
      break;
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

Expr* parse_arith_expr(token (*lexer)()) {
  stack_releaser releaser;
  token t;
  bool expecting_number = true;
  while (t = lexer(), t.type != ';' && t.type) {
    if (expecting_number ^
          (t.type == INTEGER_LITERAL || || t.type == '('
           || is_unary_operator(t.type))) {
      throw domain_error("Consecutive numbers or operators found.");
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
          throw domain_error("Unmatched right parenthesis in input.\n");
        }
        // Remove '('
        op_stack.pop_back();
        break;
      default:
        throw domain_error("Unexpected token.\n");
    }
  }
  if (expecting_number) {
    throw domain_error("Missing operand.\n");
  }
  while (!op_stack.empty()) {
    if (op_stack.back() == '(') {
      throw domain_error("Unmatched left parenthesis in input.\n");
    }
    process_last_operator();
  }

  if (num_stack.empty()) {
    // We had no input.
    return nullptr;
  }

  Expr *expr = num_stack.back();
  num_stack.pop_back();
  return expr;
}
