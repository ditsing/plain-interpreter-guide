/**
 * A simple calculator that computes as it takes input.
 */
#include <stdio.h>
#include <ctype.h>

int main() {
	printf("Ready for input.\n");

	int c;
	double first_operand = 0;
	char last_operator  = '\0';
	double operand = 0;
	while ((c = getchar()) != EOF) {
		char ch = c;
		if (isdigit(ch)) {
			operand *= 10;
			operand += ch - '0';
		} else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=') {
			switch (last_operator) {
				case '\0':first_operand = operand;
					  break;
				case '=': 
					  break;
				case '+': first_operand += operand;
					  break;
				case '-': first_operand -= operand;
					  break;
				case '*': first_operand *= operand;
					  break;
				case '/': first_operand /= operand;
					  break;
			}
			if (last_operator != '\0') {
				printf("=%.2f\n", first_operand);
			}
			last_operator = ch;
			operand = 0;
		}
    // Homework: when receiving 2 newline charactors, clear remembered result.
	}

	return 0;
}
