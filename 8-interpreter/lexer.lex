%{
#include "lexer.h"

token yylval;
int yycharno = 1;

#define COUNT_CHAR { yycharno += yyleng; } while(0)
#define YY_USER_ACTION COUNT_CHAR;
%}

id          [[:alpha:]_][[:alnum:]_]*
int_const   0|([1-9][[:digit:]]*)
operator    [+\-*/%^~();=]
space       [ \t\r]

%%

"//"[^\n]*    {
  // Comment
}

"int"         {
  yylval.str_val = yytext;
  return K_INTEGER_TYPE;
}

"double"      {
  yylval.str_val = yytext;
  return K_DOUBLE_TYPE;
}

"print"       {
  yylval.str_val = yytext;
  return K_PRINT;
}

{id}          {
  yylval.str_val = yytext;
  return IDENTIFIER;
}

{int_const}   {
  yylval.int_val = atoi(yytext);
  return INTEGER_LITERAL;
}

{operator}    {
  yylval.ops_val = *yytext;
  return *yytext;
}

{space}       {
}

\n            {
  yycharno = 1;
  return ';';
}

.             {
  yylval.err_val = *yytext;
  return ERROR_LEXEME;
}

%%

token lexer() {
  int ret = yylex();

  token t = yylval;
  t.line = yylineno;
  t.column = yycharno - yyleng;
  t.type = ret;

  return t;
}
