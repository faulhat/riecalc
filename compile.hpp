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

using namespace asmjit;

typedef double (*Func)(double);

/* A type for the REPL's symbol table */
class FnTable : public std::unordered_map<std::string, Func> {
public:
   FnTable();
};

typedef std::unordered_map<std::string, double> VarTable;

/* An exception with a method that prints a message */
class ReportingException : public std::exception {
public:
   virtual void report();
};

/* Name resolution failure exception class.
 * Thrown by the compiler when the user tries to use an unknown function.
 */
class NameResFail : public ReportingException {
public:
   std::string name;

   NameResFail(const char *name);

   virtual const char *what();
   
   virtual void report();
};

class ParseError : public ReportingException {
public:
   std::string msg;

   ParseError(char *msg);

   virtual const char *what();

   virtual void report();
};

struct ExecCtx {
   FnTable fnTable;
   VarTable varTable;
};

/* A class to store information for the compiler.
 * Meant to be used for the compilation of a single expression.
 */
class CompCtx {
public:
   const ExecCtx &ectx;
   x86::Compiler cc;
   x86::Xmm y, x;

   JitRuntime &rt;
   CodeHolder &code;

   CompCtx(JitRuntime &rt,
           CodeHolder &code,
           const ExecCtx &ectx);

   /**
    * Starts the recursive compilation of the expression.
    *
    * @param expr The expression to compile
    */
   void conv_expr_rec(const Expr *expr);

   /**
    * Finalizes the compiler
    *
    * @return The compiled function
    */
   Func end();

private:
   void conv_unary(const Unary *unary);

   void conv_binary(const Binary *binary);

   void conv_apply(const Apply *apply);

   void conv_var_expr(const char *varname);
};

/**
 * Converts the provided expression into a callable function.
 *
 * @param expr The expression to compile
 * @param rt The asmjit runtime
 * @param ectx The context storing the symbol tables 
 *
 * @return The compiled function
 */
Func conv_expr(const Expr *expr,
               JitRuntime &rt,
               const ExecCtx &ectx);

/**
 * Evaluates an expression from a string.
 * Writes the result to the provided double.
 *
 * @param rt The asmjit runtime
 * @param in The input string
 * @param ectx The context storing the symbol tables
 * @param expr Where to write the parsed expression
 * @param result Where to write the result
 * @param funcRes Where to write the function name
 * @param varRes Where to write the var name
 *
 * @return true if the expression had a result (false if it was a definition).
 */
bool conv_eval_str(JitRuntime &rt,
                   const char *in,
                   ExecCtx &ectx,
                   Expr **expr,
                   double &result,
                   char **funcRes = nullptr,
                   char **varRes = nullptr);

#endif

