CC=gcc
CFLAGS=-g -Wall -Wno-deprecated-declarations -Wno-unused-function `pkg-config --cflags gtk+-3.0`

LEX=flex
YACC=bison

SRCS=compile.cpp expr.c lexer.c main.cpp parser.c repl.cpp sgraph.cpp test.cpp

full: rcalc
	echo '#!/usr/bin/env bash\nLD_LIBRARY_PATH=$$LD_LIBRARY_PATH:./asmjit/ ./rcalc.out $$@' > rcalc && chmod +x rcalc

rcalc: libasmjit lexer parser $(SRCS) compile.hpp expr.h
	$(CC) $(CFLAGS) -orcalc.out $(SRCS) -L./asmjit/ -lasmjit `pkg-config --libs gtk+-3.0` -lm -lstdc++

libasmjit: asmjit/CMakeLists.txt
	cd asmjit/ && cmake . && make

lexer: lexer.l
	$(LEX) --header-file=lexer.h -olexer.c lexer.l

parser: parser.y
	$(YACC) -d -oparser.c parser.y

