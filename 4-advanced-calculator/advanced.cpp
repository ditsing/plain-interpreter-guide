/**
 * An advanced calculator that takes in a line and evalucate it as an math
 * expression.
 */
#include <cstdio>
#include <cctype>
#include <cmath>
#include <vector>
#include <stdexcept>

using std::vector;
using std::overflow_error;

class Expr {
  static const double esp;
  public:
  virtual double evaluate() const = 0;

  protected:
  static bool is_zero(double value) {
    return value < esp and value > -esp;
  }
};

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
    /**
     * HOMEWORK 1: Short-circuit Evaluation
     *
     * Implement this method to evaluate `left` times `right`. Skip evaluating
     * the right hand side if the left hand side is 0.
     *
     * Note this does not usually happen in real world, since an if-else
     * statement is usually much more expensive than arithmetic operators.
     */
    return 0.0;
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
    /**
     * HOMEWORK 3: Exceptions
     *
     * Modify this method so that a `range_error` is thrown when instructed to
     * calculate non-integer power of negative values. For example, sqrt(-2),
     * i.e. (-2)^(1/2), does not exist within real number range.
     *
     * Make sure your program continue to accept input after range_error is
     * thrown, by catching the error in main function.
     *
     * Arithmetic functions in math.h usually return a special double value
     * `nan` when the input does not make sense. Abbreviation "nan" stands for
     * "not a number".
     */
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
  /**
   * HOMEWORK 2: Unary operators
   *
   * Add a special case for unary operators, which only takes on argument as
   * input.
   */
  // Special case for unary operators.

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

int main() {
  char ch = '\0';
  double operand = 0;
  bool error_mod = false;
  bool remaining_number = false;
  while ((ch = getchar()) != EOF) {
    if (error_mod) {
      if (ch == '\n') {
        error_mod = false;
        num_stack.clear();
        op_stack.clear();
      }
      continue;
    }
    if (isdigit(ch)) {
      operand = operand * 10 + (ch - '0');
      remaining_number = true;
    } else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^'
        || ch == '(' || ch == ')' || ch == '~') {
      /**
       * MIDTERM*: Catch all errors in input
       *
       * Consider all possible inputs and detect the invalid ones. Report error
       * if the input is invalid, with appropriate error messages.
       *
       * Come up with test data to cover as many cases as possible.
       */
      if (remaining_number) {
        // We've got a whole number.
        num_stack.push_back(new Num(operand));
        operand = 0; // Clear the input number cache.
        remaining_number = false;
      }

      // Process operators that can be processed.
      switch (ch) {
        case '(':
        case '~':
          // Do nothing.
          op_stack.push_back(ch);
          break;
        case '^':
          // Power operator has the highest priority.
          while (!op_stack.empty() &&
              (op_stack.back() == '^' || op_stack.back() == '~')) {
            process_last_operator();
          }
          op_stack.push_back(ch);
          break;
        case '*':
        case '/':
          // Process all operators that have a higher or equal priority.
          while (!op_stack.empty()
              && (op_stack.back() == '^' || op_stack.back() == '*'
               || op_stack.back() == '/' || op_stack.back() == '~')) {
            process_last_operator();
          }
          op_stack.push_back(ch);
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
          op_stack.push_back(ch);
          break;
        case ')':
          while (!op_stack.empty() && op_stack.back() != '(') {
            process_last_operator();
          }
          if (op_stack.empty()) {
            // Error: expecting parenthesis but not getting one.
            printf("Unmatched right parenthesis in input.\n");
            error_mod = true;
            continue;
          }
          // Remove '('
          op_stack.pop_back();
          break;
        default:
          // Do nothing, ignore.
          break;
      }
    } else if (ch == '\n') {
      if (remaining_number) {
        // Another number.
        num_stack.push_back(new Num(operand));
        remaining_number = false;
      }
      operand = 0; // Clear the input number cache.

      while (!op_stack.empty()) {
        process_last_operator();
      }
      try {
        printf("=%.2f\n", num_stack.back()->evaluate());
      } catch (const overflow_error &e) {
        printf("Caught overflow error: %s\n", e.what());
      }
      num_stack.clear();
      op_stack.clear(); // Should be empty anyway.
    }
  }
}
