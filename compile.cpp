#include "compile.hpp"

#include <cmath>
#include <cstdio>
#include <iostream>
#include <sstream>

using namespace asmjit;

NameResFail::NameResFail(const char *_name)
   : name(_name)
{}

const char *NameResFail::what() {
   return "Name resolution failure.";
}

void NameResFail::report() {
   printf("Function name could not be resolved.\nName given: %s\n\n", name.c_str());
}

CompCtx::CompCtx(JitRuntime &_rt, CodeHolder &_code, const Table *_table)
   : rt(_rt), code(_code), cc(&_code), table(_table)
{
   x = cc.newXmm();
   y = cc.newXmm();

   FuncNode *f = cc.addFunc(FuncSignatureT<double, double>());
   f->setArg(0, x);
}

void CompCtx::conv_var_expr_rec(const Expr *expr) {
   switch (expr->type) {
   case UNARY:
      conv_unary(expr->val.unary);
      break;
   case BINARY:
      conv_binary(expr->val.binary);
      break;
   case APPLY:
      conv_apply(expr->val.apply);
      break;
   case NUMBER:
      {
         x86::Mem numConst = cc.newDoubleConst(ConstPoolScope::kLocal, expr->val.number);
         cc.movsd(y, numConst);
      }

      break;
   case VARIABLE:
      cc.movsd(y, x);
      break;
   }
}

void CompCtx::conv_unary(const Unary *unary) {
   conv_var_expr_rec(unary->inner);
   switch (unary->op) {
   case NEG:
      {
         x86::Mem top = cc.newStack(4, 16);
         cc.movsd(top, y);
         cc.xorps(y, y);
         cc.subsd(y, top);
      }

      break;
   case ABS:
      
      InvokeNode *toAbs;
      cc.invoke(&toAbs,
                (double (*)(double))&abs,
                FuncSignatureT<double, double>());
      toAbs->setArg(0, y);
      toAbs->setRet(0, y);

      break;
   }
}

void CompCtx::conv_binary(const Binary *binary) {
   x86::Mem top = cc.newStack(4, 16);

   conv_var_expr_rec(binary->rhs);
   cc.movsd(top, y);
   conv_var_expr_rec(binary->lhs);
   switch (binary->op) {
   case ADD:
      cc.addsd(y, top);
      break;
   case SUB:
      cc.subsd(y, top);
      break;
   case MUL:
      cc.mulsd(y, top);
      break;
   case DIV:
      cc.divsd(y, top);
      break;
   case POW:
      {
         x86::Mem top2 = cc.newStack(4, 16);
         cc.movsd(top2, x);
         cc.movsd(x, top);

         InvokeNode *toPow;
         cc.invoke(&toPow,
                   (double (*)(double, double))&pow,
                   FuncSignatureT<double, double, double>());
         toPow->setArg(0, y);
         toPow->setArg(1, x);
         toPow->setRet(0, y);

         cc.movsd(x, top2);
      }

      break;
   }
}

void CompCtx::conv_apply(const Apply *apply) {
   if (table->find(apply->funcname) == table->end()) {
      throw NameResFail(apply->funcname);
   }

   std::string funcname(apply->funcname);
   conv_var_expr_rec(apply->arg);
   InvokeNode *toFn;
   cc.invoke(&toFn,
             table->at(funcname),
             FuncSignatureT<double, double>());

   toFn->setArg(0, y);
   toFn->setRet(0, y);
}

Func CompCtx::end() {
   cc.ret(y);
   cc.endFunc();
   cc.finalize();

   Func fn;
   Error err = rt.add(&fn, &code);
   if (err) {
      printf("AsmJit failed: %s\n", DebugUtils::errorAsString(err));
      exit(1);
   }

   return fn;
}

Func conv_var_expr(const Expr *expr, JitRuntime &rt, const Table *table) {
   CodeHolder code;
   code.init(rt.environment(), rt.cpuFeatures());
   
   CompCtx ctx(rt, code, table);
   ctx.conv_var_expr_rec(expr);
   
   return ctx.end();   
}

void add_default_fns(Table &table) {
   table["Sqrt"] = &sqrt;
   table["Log"] = &log;
   table["Sin"] = &sin;
   table["Cos"] = &cos;
   table["Tan"] = &tan;
}

