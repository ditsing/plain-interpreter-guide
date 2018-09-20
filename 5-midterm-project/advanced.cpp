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
using std::range_error;

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
  // Special case for uniary operators.
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

int main() {
  char ch = '\0';
  char last_ch = '\0';
  double operand = 0;
  bool error_mod = false;
  while ((ch = getchar()) != EOF) {
    if (error_mod) {
      if (ch == '\n') {
        stack_releaser releaser;
        error_mod = false;
        last_ch = '\0';
      }
      continue;
    }
    if (isdigit(ch)) {
      if (last_ch == ')') {
        printf("Expecting operators following ')' but got digit %c.\n", ch);
        error_mod = true;
        continue;
      }
      operand = operand * 10 + (ch - '0');
    } else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^'
        || ch == '(' || ch == ')' || ch == '~') {
      if (ch == '(' || is_unary_operator(ch)) {
        if (isdigit(last_ch) || last_ch == ')') {
          printf("Unexpected operator '%c' following '%c'.\n", ch, last_ch);
          error_mod = true;
          continue;
        }
      } else {
        if (!isdigit(last_ch) && last_ch != ')') {
          printf("Unexpected operator '%c' following '%c'.\n", ch, last_ch);
          error_mod = true;
          continue;
        }
      }

      if (isdigit(last_ch)) {
        // We've got a whole number.
        num_stack.push_back(new Num(operand));
        operand = 0; // Clear the input number cache.
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
      stack_releaser releaser;
      if (isdigit(last_ch)) {
        // Another number.
        num_stack.push_back(new Num(operand));
      } else if (last_ch != ')') {
        printf("Expecting last operand.\n");
        last_ch = ch;
        // Not using error_mod.
        continue;
      }
      operand = 0; // Clear the input number cache.

      bool has_error = false;
      while (!op_stack.empty()) {
        if (op_stack.back() == '(') {
          printf("Unmatched left parenthesis.\n");
          last_ch = ch;
          // Not using error_mod.
          has_error = true;
          break;
        }
        process_last_operator();
      }
      if (has_error) {
        continue;
      }

      if (num_stack.empty()) {
        // We've had no input.
        continue;
      }

      try {
        printf("=%.2f\n", num_stack.back()->evaluate());
      } catch (const overflow_error &e) {
        printf("Caught overflow error: %s\n", e.what());
      } catch (const range_error &e) {
        printf("Caught range error: %s\n", e.what());
      }
    } else {
      ch = last_ch; // Ignored. Preserve the value of last_ch.
    }
    last_ch = ch;
  }
}
