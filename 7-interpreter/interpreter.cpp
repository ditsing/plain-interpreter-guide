#include <cstdio>
#include <string>
#include <memory>
#include <stdexcept>
#include "lexer.h"
#include "arith_expr.h"
#include "interpreter.h"

using std::string;
using std::unique_ptr;

void Program::append_statement(const Statement *p) {
  statements.emplace_back(p);
}

void Program::run() {
  for (const auto &st: statements) {
    st->run();
  }
}

/**
 * HOMEWORK 2: Print statement
 *
 * How should the interpreter execute a print statement? Complete the section
 * below.
 */
void PrintStatement::run() const {
}

enum class State {
  Start,
  Print,
};

unique_ptr<Program> parse_program() {
  unique_ptr<Program> p(new Program());
  State state;
  token t;
  while ((t=lexer(), t.type)) {
    switch (t.type) {
      case K_PRINT:
        {
          if (state != State::Start) {
            THROW_ERROR_FORMAT(
                CompilingError,
                "Unexpected keyword print at line %d, column %d.",
                t.line,
                t.column);
          }
          /**
           * HOMEWORK 3: Keywork Print
           *
           * Complete the following section. What should the compiler do when
           * encountered keyword `print`?
           */
        }
        break;
      case ERROR_LEXEME:
        THROW_ERROR_FORMAT(
            CompilingError,
            "Unrecognized input %c at line %d column %d.",
            t.err_val, t.line, t.column);
      default:
        // Throwing logic_error since this is a problem with the compiler.
        THROW_ERROR_FORMAT(
            std::logic_error,
            "Unrecognized token type %d at line %d column %d.",
            t.type, t.line, t.column);
        break;
    }
  }

  return p;
}

int main() {
  try {
    unique_ptr<Program> p = parse_program();

    // Parsing finished, now we run the program.
    p->run();
  } catch (const CompilingError &e) {
    printf("Compiling error: %s\n", e.what());
    return -1;
  } catch (const RuntimeError &e) {
    printf("Runtime error: %s\n", e.what());
    return -1;
  }
}
