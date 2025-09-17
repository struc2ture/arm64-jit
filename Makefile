CC = clang
CFLAGS  = -g
CFLAGS += -Wall -Wextra -Werror -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable
LFLAGS  =
SRC = src

default: test2

all: bin/basic_test bin/test2

BASIC_TEST_DEP = jit.c basic_expr.c
bin/basic_test: $(wildcard src/*)
	$(CC) $(CFLAGS) $(SRC)/basic_test.c $(addprefix $(SRC)/, $(BASIC_TEST_DEP)) -o $@

basic_test: bin/basic_test
	lldb ./bin/basic_test -o r -o q

TEST2 = jit.c lexer.c parser.c codegen.c
bin/test2: $(wildcard src/*)
	$(CC) $(CFLAGS) $(SRC)/test2.c $(addprefix $(SRC)/, $(TEST2)) -o $@

test2: bin/test2
	lldb ./bin/test2 -o r -o q

