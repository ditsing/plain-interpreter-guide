#include <cstdio>
#include <string>
#include <memory>
#include <stdexcept>
#include "lexer.h"
#include "arith_expr.h"
#include "interpreter.h"
#include "variable.h"

using std::string;
using std::unique_ptr;

const Variable &Program::lookup_variable(const string &name) const {
  if (!defined_variable(name)) {
    throw CompilingError("Variable " + name + " does not exist.");
  }
  return variable_map.at(name);
}

Variable &Program::lookup_variable(const string &name) {
  if (!defined_variable(name)) {
    throw CompilingError("Variable " + name + " does not exist.");
  }
  return variable_map.at(name);
}

/**
 * HOMEWORK 5: Variable Management API
 *
 * This is the first time we ever implement an Appliation Programming Interface,
 * API for short. An API describes how other parts of a program intereact with
 * one part, how data flows between them and what responsibility each part
 * carries for the data.
 *
 * To control a car, we have a stering wheel for the direction, and paddles for
 * the speed. Similarly, to manage books we ask a librarian to
 * 1. find a book by name,
 * 2. fetch the book from the shelves,
 * 3. put the book back to the shelves.
 * 4. add a new book to the shelves, and remeber where it is placed.
 * In this scenario, the librarian serves as the API of managing books.
 *
 * By defining an API, we draw a clear boundray between parts of our programs,
 * which leads to better separation and de-coupling.
 *
 * Implement the following two methods as part of the variable management API.
 */
bool Program::defined_variable(const string &name) const {
}

Variable &Program::create_variable(const string &name, Expr::Type type) {
}

void Program::append_statement(const Statement *p) {
  statements.emplace_back(p);
}

void Program::run() {
  for (auto &pair: variable_map) {
    pair.second.reset();
  }
  for (const auto &st: statements) {
    st->run();
  }
}

void Assignment::run() const {
  // Unpacking.
  switch (var.type()) {
    case Expr::Type::Int:
      {
        int x;
        switch (expr->type()) {
          case Expr::Type::Int:
            x = expr->evaluate_to_int();
            break;
          case Expr::Type::Double:
            // Should be truncating.
            x = expr->evaluate_to_double();
          default:
            THROW_ERROR_FORMAT(
                std::logic_error,
                "Unknown expr value type %d",
                (int) var.type());
        }
        var.assign(x);
        break;
      }
    case Expr::Type::Double:
      {
        var.assign(expr->evaluate_and_promote_to_double());
        break;
      }
    default:
      THROW_ERROR_FORMAT(
          std::logic_error,
          "Unknown value type %d",
          (int) var.type());
  }
}

void PrintStatement::run() const {
  if (expr->type() == Expr::Type::Int) {
    printf("%d\n", expr->evaluate_to_int());
  } else {
    printf("%.2lf\n", expr->evaluate_to_double());
  }
}

enum class State {
  Start,
  TypeDecl,
  Initialize,
  Assign,
  Print,
};

unique_ptr<Program> parse_program() {
  unique_ptr<Program> p(new Program());
  string idt;
  Expr::Type type_decl;
  State state;
  token t;
  while ((t=lexer(), t.type)) {
    switch (t.type) {
      case K_INTEGER_TYPE:
        {
          if (state != State::Start) {
            THROW_ERROR_FORMAT(
                CompilingError,
                "Unexpected keyword %s at line %d, column %d.",
                t.str_val,
                t.line,
                t.column);
          }
          state = State::TypeDecl;
          type_decl = Expr::Type::Int;
        }
        break;
      case K_DOUBLE_TYPE:
        {
          if (state != State::Start) {
            THROW_ERROR_FORMAT(
                CompilingError,
                "Unexpected keyword %s at line %d, column %d.",
                t.str_val,
                t.line,
                t.column);
          }
          state = State::TypeDecl;
          type_decl = Expr::Type::Double;
        }
        break;
      case IDENTIFIER:
        {
          /**
           * HOMEWORK 3: Token IDENTIFIER
           *
           * We could be creating a new variable, or assigning a value to an
           * existing one. Following the identifier we always have a `=`. Try to
           * unify the Initialization case and the Assignment case.
           */
        }
        break;
      case '=':
        {
          if (state == State::Initialize) {
            /**
             * HOMEWORK 4: Variable Initialization
             *
             * We are in the middle of creating a variable. Before `=` we had
             * the type and the variable name. Call `parse_arith_expr` to get
             * the expression that will be used to compute the initial value.
             *
             * The assignment case can be used as reference.
             */
          } else if (state == State::Assign) {
            Expr *e = parse_arith_expr(lexer, p.get());
            if (e == nullptr) {
              THROW_ERROR_FORMAT(
                  CompilingError,
                  "Excpeting an expression to assign to variable %s at line %d, column %d.",
                  idt.c_str(),
                  t.line,
                  t.column);
            }
            if (!p->defined_variable(idt)) {
              THROW_ERROR_FORMAT(
                  CompilingError,
                  "Undefined variable %s at line %d, column %d.",
                  idt.c_str(),
                  t.line,
                  t.column);
            }
            Variable &var(p->lookup_variable(idt));
            p->append_statement(new Assignment(std::move(idt), e, var));
            state = State::Start;
          } else {
            THROW_ERROR_FORMAT(
                CompilingError,
                "Unexpected operator %c = at line %d, column %d.",
                t.ops_val,
                t.line,
                t.column);
          }
        }
        break;
      case K_PRINT:
        {
          if (state != State::Start) {
            THROW_ERROR_FORMAT(
                CompilingError,
                "Unexpected keyword print at line %d, column %d.",
                t.line,
                t.column);
          }
          Expr *e = parse_arith_expr(lexer, p.get());
          if (e == nullptr) {
            THROW_ERROR_FORMAT(
                CompilingError,
                "Excpeting an expression to print at line %d, column %d.",
                t.line,
                t.column);
          }
          p->append_statement(new PrintStatement(e));
          state = State::Start;
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
