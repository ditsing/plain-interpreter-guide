#ifndef LEXER_H
#define LEXER_H

enum token_type {
  // 1 ~ 255 reserved for regular chars.
  IDENTIFIER = 257,
  INTEGER_LITERAL,

  K_INTEGER_TYPE,
  K_DOUBLE_TYPE,
  K_PRINT,

  ERROR_LEXEME
};

typedef struct token {
  int line;
  int column;
  int type; // the value usually is enum token_type.
  union {
    char *str_val;
    int int_val;
    char ops_val;
    char err_val;
  };
} token;

token lexer();
#endif
