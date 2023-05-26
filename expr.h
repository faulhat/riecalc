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
} UExpr;

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
} BExpr;

typedef enum {
   NUMBER, UNARY, BINARY
} ExprType;

typedef union {
   float number;
   UExpr *unary;
   BExpr *binary;
} ExprVal;

struct Expr {
   ExprType type;
   ExprVal val;
};

Expr *new_num_expr(float number);

Expr *new_unary(UOp op, Expr *inner);

Expr *new_binary(BOp op, Expr *lhs, Expr *rhs);

float eval_const_expr(const Expr *expr);

void print_expr(const Expr *expr, FILE *to);

void destroy_expr(Expr *expr);

#endif

