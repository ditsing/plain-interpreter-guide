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
      throw RuntimeError("Divded by zero");
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
      throw RuntimeError(
          "Cannot calculate non-integer power of negative value");
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

Expr* parse_arith_expr(token (*lexer)()) {
  stack_releaser releaser;
  token t;
  bool expecting_number = true;
  while (t = lexer(), t.type != ';' && t.type) {
    // There are two states when parsing the expression, one is when we expects
    // an operand in the next input, the other is when we expects an operator.
    //
    // Intially we start with an operand. The operand can be a number, or any
    // expression enclosed in parentheses, or an expession prepeneded with a
    // uniary operator, i.e. `~`. After the first operand, there must be an
    // operator `+-*/^`. Then we need an operand again and the loop continues.
    //
    // The situation can be more complicated if we decide to support suffix
    // uniary operators, e.g. factorial operator `!`.
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
        /**
         * HOMEWORK 1: Integer Literal
         *
         * Complete this section with actions need to be taken when the lexer
         * returns an interger literal.
         */
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
      return nullptr;
    }
    throw CompilingError("Missing operand");
  }

  // Process all remaining operators in the stack.
  while (!op_stack.empty()) {
    if (op_stack.back() == '(') {
      throw CompilingError("Unmatched left parenthesis in input");
    }
    process_last_operator();
  }

  // We should have exactly one expression left.
  // Throwing an logic_error if it is not the case might be more appropriate.
  Expr *expr = num_stack.back();
  num_stack.pop_back();
  return expr;
}
