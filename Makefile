CC = clang
CFLAGS  = -g
CFLAGS += -Wall -Wextra -Werror -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable
LFLAGS  =
SRC = src

default: basic_test

basic_test: bin/basic_test
	lldb ./bin/basic_test -o run -o q

BASIC_TEST_DEP = jit.c basic_expr.c
bin/basic_test: $(wildcard src/*)
	$(CC) $(CFLAGS) $(SRC)/basic_test.c $(addprefix $(SRC)/, $(BASIC_TEST_DEP)) -o $@
