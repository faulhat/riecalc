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

typedef struct {
   char *funcname;
   Expr *arg;
} Apply;

typedef enum {
   NUMBER, VARIABLE, UNARY, BINARY, APPLY
} ExprType;

typedef union {
   double number;
   Unary *unary;
   Binary *binary;
   Apply *apply;
} ExprVal;

struct Expr {
   ExprType type;
   ExprVal val;
};

Expr *new_num_expr(double number);

Expr *new_var_expr();

Expr *new_unary(UOp op, Expr *inner);

Expr *new_binary(BOp op, Expr *lhs, Expr *rhs);

Expr *new_apply(char *funcname, Expr *arg);

double eval_const_expr(const Expr *expr);

void print_expr(const Expr *expr, FILE *to);

void destroy_expr(Expr *expr);

#endif

