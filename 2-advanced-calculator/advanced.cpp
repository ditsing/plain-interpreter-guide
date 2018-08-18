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

/**
 * HOMEWORK 3*
 * During testing you should have noticed that "^2" and "+1*2" are both valid
 * expressions for our current implementation. When an operand is not supplied,
 * it is considered 0. This can be confusing because "1/" will throw an "divided
 * by 0" error.
 * A good program should detect defects in input. How do we do that?
 */

int main() {
  char ch = '\0';
  double operand = 0;
  while ((ch = getchar()) != EOF) {
    if (isdigit(ch)) {
      operand = operand * 10 + (ch - '0');
    } else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^') {
      // We've got a whole number.
      num_stack.push_back(operand);
      operand = 0; // Clear the input number cache.

      // Process operators that can be processed.
      switch (ch) {
        case '^':
          // Power operator has the highest priority.
          while (!op_stack.empty() && op_stack.back() == '^') {
            process_last_operator();
          }
          op_stack.push_back(ch);
          break;
        case '*':
        case '/':
          /**
           * HOMEWORK 1
           * Complete this section.
           */
          break;
        case '+':
        case '-':
          // All other operators have higher priorities.
          while (!op_stack.empty()) {
            process_last_operator();
          }
          op_stack.push_back(ch);
          break;
        default:
          // Do nothing, ignore.
          break;
      }
    } else if (ch == '\n') {
      /**
       * HOMEWORK 2
       * Complete this section.
       * When seeing an newline charactor, we know that the input is complete
       * and now it is the time to process the whole expression.
       * Print the result after calculation.
       * Clear all data structures so that we are all set for next.
       */
    }
  }
}
