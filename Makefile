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

CC = gcc
CXX = g++
CFLAGS = -g -Wall

LEX = flex
YACC = bison

SRC = src
VPATH = $(SRC)
OUT = out
OBJ = obj
BIN = bin

C_OBJS = $(OBJ)/expr.o
CXX_OBJS = $(addprefix $(OBJ)/, compile.o repl.o asymptotes.o test.o)
GTK_OBJS = $(OBJ)/grapher.o $(OBJ)/main.o
OUT_OBJS = $(OBJ)/parser.o $(OBJ)/lexer.o
ALL_OBJS = $(OUT_OBJS) $(C_OBJS) $(CXX_OBJS) $(GTK_OBJS)

all : $(BIN)/rcalc

$(BIN)/rcalc : asmjit/libasmjit.so $(ALL_OBJS) $(BIN)
	$(CC) -o $@ $(ALL_OBJS) -Lasmjit/ -lasmjit `pkg-config --libs gtk+-3.0` -lm -lstdc++	

$(C_OBJS) : $(OBJ)/%.o : $(SRC)/%.c | $(OBJ)
	$(CC) $(CFLAGS) -c $^ -o $@

$(CXX_OBJS) : $(OBJ)/%.o : $(SRC)/%.cpp | $(OBJ)
	$(CXX) $(CFLAGS) -c $^ -o $@

$(OUT_OBJS) : $(OBJ)/%.o : $(OUT)/%.c | $(OBJ)
	$(CC) $(CFLAGS) -Wno-unused-function -c $^ -o $@

$(GTK_OBJS) : $(OBJ)/%.o : $(SRC)/%.cpp | $(OBJ)
	$(CC) $(CFLAGS) -Wno-deprecated-declarations `pkg-config --cflags gtk+-3.0 ` -c $^ -o $@

$(OBJ)/expr.o : | expr.h

$(OBJ)/asymptotes.o : | asymptotes.hpp
$(OBJ)/compile.o : | compile.hpp
$(OBJ)/grapher.o : | grapher.hpp asymptotes.hpp
$(OBJ)/main.o : | grapher.hpp
$(OBJ)/repl.o : | compile.hpp
$(OBJ)/test.o : | expr.h compile.hpp

asmjit/libasmjit.so : asmjit/CMakeLists.txt
	cd asmjit/ && cmake . && make

$(OUT)/parser.c : $(SRC)/parser.y | $(OUT)/lexer.c
	$(YACC) --header=$(OUT)/parser.h -o $@ $^

$(OUT)/lexer.c : $(SRC)/lexer.l | $(OUT)
	$(LEX) --header-file=$(OUT)/lexer.h -o $@ $^

$(OUT) $(OBJ) $(BIN) :
	mkdir -p $@

.PHONY : clean all
clean :
	rm -rf $(OUT)/ $(OBJ)/ $(BIN)/ asmjit/libasmjit.so

