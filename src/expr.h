/*
 * This file is part of the Riemann Project.
 * Developed by Tom Faulhaber for personal use.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>. 
 */

#ifndef EXPR_H
#define EXPR_H

#include <stdio.h>

typedef struct Expr Expr;

typedef enum {
   NEG, ABS
} UOp;

typedef struct {
   UOp op;
   Expr *inner;
} Unary;

typedef enum {
   ADD = '+',
   SUB = '-',
   MUL = '*',
   DIV = '/',
   POW = '^'
} BOp;

typedef struct {
   BOp op;
   Expr *lhs;
   Expr *rhs;
} Binary;

/* Function application expression type */
typedef struct {
   char *funcname;
   Expr *arg;
} Apply;

typedef enum {
   NUMBER, VARIABLE, ARGUMENT, UNARY, BINARY, APPLY
} ExprType;

typedef union {
   double number;
   Unary *unary;
   Binary *binary;
   Apply *apply;
   char *varname;
} ExprVal;

/* Overarching expression type */
struct Expr {
   ExprType type;
   ExprVal val;
};

/**
 * Constructor for an expression representing a literal scalar value
 */
Expr *new_num_expr(double number);

/**
 * Constructor for an expression representing x
 */
Expr *new_arg_expr();

/**
 * Constructor for an expression representing a unary operation
 *
 * @param op The operator
 * @param inner The operand
 */
Expr *new_unary(UOp op, Expr *inner);

/**
 * Constructor for an expression representing a binary operation
 *
 * @param op The operator
 * @param lhs The left-hand side operand
 * @param rhs The right-hand side operand
 */
Expr *new_binary(BOp op, Expr *lhs, Expr *rhs);

/**
 * Constructor for an expression representing a function application
 *
 * @param funcname The name of the function being applied
 * @param arg The argument passed into the function (the x value)
 */
Expr *new_apply(char *funcname, Expr *arg);

/**
 * Constructor for an expression representing a named variable
 */
Expr *new_var_expr(char *varname);

/**
 * Pretty-prints an expression to the console.
 *
 * @param expr The expression to print
 * @param to The file handle to write to
 */
void print_expr(const Expr *expr, FILE *to);

/**
 * Recursively frees parse tree.
 */
void destroy_expr(Expr *expr);

#endif

