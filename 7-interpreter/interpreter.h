#ifndef INTERPRETER_H
#define INTERPRETER_H
#include <string>
#include <vector>
#include <stdexcept>
#include <memory>
#include "arith_expr.h"

class Statement;
class Program;

class Program {
  std::vector<std::unique_ptr<const Statement>> statements;
  public:
  void append_statement(const Statement *st);

  void run();
};

class Statement {
  public:
  virtual void run() const = 0;
  virtual ~Statement() {}
};

class PrintStatement: public Statement {
  const std::unique_ptr<const Expr> expr;
  public:
  PrintStatement(const Expr *expr) : expr(expr) {}

  void run() const override;
};

#define THROW_ERROR_FORMAT(error_type, ...) {\
  char *error_msg; \
  asprintf(&error_msg, __VA_ARGS__); \
  throw error_type(error_msg); \
}

#define THROW_ERROR_FORMAT_LINE(error_type, token, ...) {\
  char *error_msg; \
  asprintf(&error_msg, __VA_ARGS__); \
  char *error_msg_2; \
  asprintf(&error_msg_2, "%s at line %d column %d.", error_msg, token.line, token.column); \
  throw error_type(error_msg_2); \
}

class CompilingError: public std::logic_error {
  public:
  CompilingError(const std::string &what) : std::logic_error(what) {}
  CompilingError(const char *what) : std::logic_error(what) {}
};

class RuntimeError: public std::logic_error {
  public:
  RuntimeError(const std::string &what) : std::logic_error(what) {}
  RuntimeError(const char *what) : std::logic_error(what) {}
};

#endif
