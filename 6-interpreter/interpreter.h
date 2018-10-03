#ifndef INTERPRETER_H
#define INTERPRETER_H
#include <stdexcept>

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
