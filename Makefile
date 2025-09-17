run: bin/main
	lldb ./bin/main -o run

bin/main: src/main.c
	clang -g -o bin/main src/main.c