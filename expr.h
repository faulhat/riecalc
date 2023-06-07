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

/* Constructors for different types of expressions. */

Expr *new_num_expr(double number);

Expr *new_arg_expr();

Expr *new_unary(UOp op, Expr *inner);

Expr *new_binary(BOp op, Expr *lhs, Expr *rhs);

Expr *new_apply(char *funcname, Expr *arg);

Expr *new_var_expr(char *varname);

/* Pretty-prints an expression to the console. */
void print_expr(const Expr *expr, FILE *to);

/* Recursively frees parse tree. */
void destroy_expr(Expr *expr);

#endif

