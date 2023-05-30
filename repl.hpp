#ifndef REPL_HPP
#define REPL_HPP

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

typedef float (*Func)(float);

typedef std::unordered_map<std::string, Func> Table;

class NameResFail : public std::exception {
public:
   std::string name;

   NameResFail(const char *name);

   virtual const char *what();
};

class NotYetImplemented : public std::exception {
public:
   virtual const char *what();
};

struct CompCtx {
   const Table *table;
   x86::Compiler cc;
   x86::Xmm y, x;

   JitRuntime &rt;
   CodeHolder &code;

   CompCtx(JitRuntime &rt, CodeHolder &code, const Table *table);

   void conv_var_expr_rec(const Expr *expr);

   Func end();

private:
   void conv_unary(const UExpr *unary);

   void conv_binary(const BExpr *binary);

   void conv_apply(const Apply *apply);
};

#endif

