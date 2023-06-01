#ifndef COMPILE_HPP
#define COMPILE_HPP

extern "C" {
   #include "expr.h"
   #include "parser.h"
   #include "lexer.h"
}

#include "asmjit/src/asmjit/x86.h"
#include <unordered_map>
#include <string>
#include <exception>
#include <stack>

using namespace asmjit;

typedef double (*Func)(double);

/* A type for the REPL's symbol table */
typedef std::unordered_map<std::string, Func> Table;

/* Name resolution failure exception class.
 * Thrown by the compiler when the user tries to use an unknown function.
 */
class NameResFail : public std::exception {
public:
   std::string name;

   NameResFail(const char *name);

   virtual const char *what();

   void report();
};

/* A class to store information for the compiler.
 * Meant to be used for the compilation of a single expression.
 */
struct CompCtx {
   const Table *table;
   x86::Compiler cc;
   x86::Xmm y, x;

   JitRuntime &rt;
   CodeHolder &code;

   CompCtx(JitRuntime &rt, CodeHolder &code, const Table *table);

   /* Starts the recursive compilation of the expression. */
   void conv_var_expr_rec(const Expr *expr);

   /* Cleans up the compiler and returns the compiled function */
   Func end();

private:
   void conv_unary(const Unary *unary);

   void conv_binary(const Binary *binary);

   void conv_apply(const Apply *apply);
};

/* Converts the provided expression into a callable function. */
Func conv_var_expr(const Expr *expr, JitRuntime &rt, const Table *table);

/* Adds common math functions into the given symbol table. */
void add_default_fns(Table &table);

#endif

