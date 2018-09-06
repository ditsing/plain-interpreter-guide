/**
 * An advanced calculator that takes in a line and evalucate it as an math
 * expression.
 */
#include <cstdio>
#include <cctype>
#include <cmath>
#include <vector>

using std::vector;

vector<char> op_stack;
vector<double> num_stack;

/**
 * Process the last operator in the stack.
 *
 * It is guaranteed that when this function is called, there is always at least
 * one operator in op_stack, and two numbers in num_stack.
 */
void process_last_operator() {
  // Takes 2 numbers from stack.
  double right_operand = num_stack.back();
  num_stack.pop_back();
  double left_operand = num_stack.back();
  num_stack.pop_back();

  // Calculation based on the current operator.
  double result = 0;
  switch (op_stack.back()) {
    case '+':
      result = left_operand + right_operand;
      break;
    case '-':
      result = left_operand - right_operand;
      break;
    case '*':
      result = left_operand * right_operand;
      break;
    case '/':
      result = left_operand / right_operand;
      break;
    case '^':
      result = pow(left_operand, right_operand);
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
  /**
   * HOMEWORK 2: Errors in input
   * 
   * Humans make mistakes and computers help correct them. You may have noticed
   * that our program produces wired result when the input is illegal. In some
   * cases we try to pop an element out of an empty stack, and the program
   * crashes. Instead, a good calculator should reject illegal inputs and point
   * out what is wrong.
   *
   * Dealing with all errors in input is hard. In this homework, make sure our
   * calculator continues to work if there are more right parentheses than left.
   *
   * A variable `error_mod` has been created for you.
   */
  while ((ch = getchar()) != EOF) {
    if (isdigit(ch)) {
      operand = operand * 10 + (ch - '0');
    } else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^'
        || ch == '(' || ch == ')') {
      /**
       * HOMEWORK 1: Parentheses
       *
       * Allow parentheses in the input. Parentheses have the highest priority.
       * Note that we used to expect one number before each opeartor. It stops
       * being the case after parentheses are introduced.
       *
       * You may have to change multiple places, not necessarily in this setion.
       */
      // We've got a whole number.
      num_stack.push_back(operand);
      operand = 0; // Clear the input number cache.

      // Process operators that can be processed.
      switch (ch) {
        case '(':
          // Do nothing.
          op_stack.push_back(ch);
          break;
        case '^':
          // Power operator has the highest priority.
          while (!op_stack.empty() && op_stack.back() == '^') {
            process_last_operator();
          }
          op_stack.push_back(ch);
          break;
        case '*':
        case '/':
          // Process all operators that have a higher or equal priority.
          while (!op_stack.empty()
              && (op_stack.back() == '^' || op_stack.back() == '*'
               || op_stack.back() == '/')) {
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
                 || op_stack.back() == '-')) {
            process_last_operator();
          }
          op_stack.push_back(ch);
          break;
        case ')':
          // HOMEWORK 1: fill in.
          break;
        default:
          // Do nothing, ignore.
          break;
      }
    } else if (ch == '\n') {
      // Another number.
      num_stack.push_back(operand);
      operand = 0; // Clear the input number cache.

      while (!op_stack.empty()) {
        process_last_operator();
      }
      printf("=%.2f\n", num_stack.back());
      num_stack.clear();
      op_stack.clear(); // Should be empty anyway.
    }
  }
}
