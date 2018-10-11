# Interpreter
interpreter: lexer.o interpreter.h interpreter.cpp lexer.h arith_expr.h arith_expr.o variable.h
	$(CXX) interpreter.cpp lexer.o arith_expr.o -o interpreter -std=c++11
lexer.cc: lexer.lex
	flex --noyywrap --yylineno -o lexer.cc lexer.lex
lexer.o: lexer.cc lexer.h
	$(CXX) -c lexer.cc -o lexer.o
arith_expr.o: arith_expr.cpp arith_expr.h interpreter.h variable.h
	$(CXX) -c arith_expr.cpp -o arith_expr.o -std=c++11
lexer_test: lexer.cc lexer.h lexer_test.c
	cp lexer.cc lexer.c
	$(CC) lexer.c lexer_test.c -o lexer_test
	$(RM) lexer.c
.PHONY:test_lexer
test_lexer: lexer_test
	./lexer_test

clean:
	$(RM) lexer.cc lexer.o arith_expr.o lexer_test interpreter
