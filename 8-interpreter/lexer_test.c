#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "lexer.h"
#define RETURN_STR(...) { \
  error_str r; \
  r.len = asprintf(&(r.str), __VA_ARGS__); \
  return r; \
}
#define RETURN_SUCCESS { return SUCCESS; }
#define ASSERT_NEXT_TOKEN(func, value) { \
  error_str r = func(value, lexer()); \
  if (r.len != 0) { \
    char *msg = r.str; \
    r.len = asprintf(&(r.str), "At %s line %d: %s", __FILE__, __LINE__, msg); \
    return r; \
  } \
}
#define RUN_TEST(func) { \
  printf("Test %s running...\n", #func); \
  error_str r = func(); \
  if (r.len != 0) { \
    printf("Test %s failed: %s\n", #func, r.str); \
  } else { \
    printf("Test %s passed.\n", #func); \
  } \
}

char *TEST_1="1/3*17-(2^3)+5;";
char *TEST_2="(1/3*17)\n"; // ends with new line.
char *TEST_3="12345/23-453281\n";

typedef struct error_str {
  int len;
  char *str;
} error_str;
const error_str SUCCESS = {0, NULL};

void yyrestart(FILE *);
void setup_test(char *input) {
  FILE *file = fmemopen(input, strlen(input), "r");
  yyrestart(file);
}

error_str is_null(int dummy, token t) {
  if (t.type != 0) {
    RETURN_STR("Expecting EOF, but got %d.", t.type);
  }
  RETURN_SUCCESS;
}

error_str is_char(char c, token t) {
  if (t.type != c) {
    RETURN_STR("Expecting %c but got %d.", c, t.type);
  }
  RETURN_SUCCESS;
}

error_str is_int(int i, token t) {
  if (t.type != INTEGER_LITERAL) {
    RETURN_STR("Expecting integer %d but got type %d.", i, t.type);
  }
  if (t.int_val != i) {
    RETURN_STR("Expecting integer %d but got %d.", i, t.int_val);
  }
  RETURN_SUCCESS;
}

error_str test_1() {
  setup_test(TEST_1);
  ASSERT_NEXT_TOKEN(is_int, 1);
  ASSERT_NEXT_TOKEN(is_char, '/');
  ASSERT_NEXT_TOKEN(is_int, 3);
  ASSERT_NEXT_TOKEN(is_char, '*');
  ASSERT_NEXT_TOKEN(is_int, 17);
  ASSERT_NEXT_TOKEN(is_char, '-');
  ASSERT_NEXT_TOKEN(is_char, '(');
  ASSERT_NEXT_TOKEN(is_int, 2);
  ASSERT_NEXT_TOKEN(is_char, '^');
  ASSERT_NEXT_TOKEN(is_int, 3);
  ASSERT_NEXT_TOKEN(is_char, ')');
  ASSERT_NEXT_TOKEN(is_char, '+');
  ASSERT_NEXT_TOKEN(is_int, 5);
  ASSERT_NEXT_TOKEN(is_char, ';');
  ASSERT_NEXT_TOKEN(is_null, 0);
  RETURN_SUCCESS;
}

error_str test_newline() {
  setup_test(TEST_2);
  ASSERT_NEXT_TOKEN(is_char, '(');
  ASSERT_NEXT_TOKEN(is_int, 1);
  ASSERT_NEXT_TOKEN(is_char, '/');
  ASSERT_NEXT_TOKEN(is_int, 3);
  ASSERT_NEXT_TOKEN(is_char, '*');
  ASSERT_NEXT_TOKEN(is_int, 17);
  ASSERT_NEXT_TOKEN(is_char, ')');
  ASSERT_NEXT_TOKEN(is_char, ';');
  ASSERT_NEXT_TOKEN(is_null, 0);
  RETURN_SUCCESS;
}

error_str test_large_number() {
  setup_test(TEST_3);
  ASSERT_NEXT_TOKEN(is_int, 12345);
  ASSERT_NEXT_TOKEN(is_char, '/');
  ASSERT_NEXT_TOKEN(is_int, 23);
  ASSERT_NEXT_TOKEN(is_char, '-');
  ASSERT_NEXT_TOKEN(is_int, 453281);
  ASSERT_NEXT_TOKEN(is_char, ';');
  ASSERT_NEXT_TOKEN(is_null, 0);
  RETURN_SUCCESS;
}

int main() {
  RUN_TEST(test_1);
  RUN_TEST(test_newline);
  RUN_TEST(test_large_number);
  return 0;
}
