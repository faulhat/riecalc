#
# This file is part of the Riemann Project.
# Developed by Tom Faulhaber for personal use.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>. 
#

CC=gcc
CFLAGS=-g -Wall -Wno-deprecated-declarations -Wno-unused-function `pkg-config --cflags gtk+-3.0`

LEX=flex
YACC=bison

SRCS=compile.cpp expr.c main.cpp repl.cpp grapher.cpp asymptotes.cpp test.cpp

rcalc: mkdirs libasmjit lexer parser src/ 
	cd src && $(CC) $(CFLAGS) -o../bin/rcalc $(SRCS) ../out/parser.c ../out/lexer.c -L../asmjit/ -lasmjit `pkg-config --libs gtk+-3.0` -lm -lstdc++

libasmjit: asmjit/CMakeLists.txt
	cd asmjit/ && cmake . && make

lexer: src/lexer.l
	$(LEX) --header-file=out/lexer.h -oout/lexer.c src/lexer.l

parser: src/parser.y
	$(YACC) --header=out/parser.h -oout/parser.c src/parser.y

mkdirs:
	mkdir -p out && mkdir -p bin

