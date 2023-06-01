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
   return "Function name could not be resolved!\n";
}

const char *NotYetImplemented::what() {
   return "Operation not yet implemented!\n";
}

CompCtx::CompCtx(JitRuntime &_rt, CodeHolder &_code, const Table *_table)
   : rt(_rt), code(_code), cc(&_code), table(_table)
{
   x = cc.newXmm();
   y = cc.newXmm();

   FuncNode *f = cc.addFunc(FuncSignatureT<float, float>());
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
         x86::Mem numConst = cc.newFloatConst(ConstPoolScope::kLocal, expr->val.number);
         cc.movss(y, numConst);
      }

      break;
   case VARIABLE:
      cc.movss(y, x);
      break;
   }
}

void CompCtx::conv_unary(const Unary *unary) {
   conv_var_expr_rec(unary->inner);
   switch (unary->op) {
   case NEG:
      {
         x86::Mem top = cc.newStack(4, 16);
         cc.movss(top, y);
         cc.xorps(y, y);
         cc.subss(y, top);
      }

      break;
   case ABS:
      cc.cvtss2sd(y, y);
      
      InvokeNode *toAbs;
      cc.invoke(&toAbs,
                (double (*)(double))&abs,
                FuncSignatureT<double, double>());
      toAbs->setArg(0, y);
      toAbs->setRet(0, y);

      cc.cvtsd2ss(y, y);
      break;
   }
}

void CompCtx::conv_binary(const Binary *binary) {
   x86::Mem top = cc.newStack(4, 16);

   conv_var_expr_rec(binary->rhs);
   cc.movss(top, y);
   conv_var_expr_rec(binary->lhs);
   switch (binary->op) {
   case ADD:
      cc.addss(y, top);
      break;
   case SUB:
      cc.subss(y, top);
      break;
   case MUL:
      cc.mulss(y, top);
      break;
   case DIV:
      cc.divss(y, top);
      break;
   case POW:
      {
         x86::Mem top2 = cc.newStack(4, 16);
         cc.movss(top2, x);
         cc.movss(x, top);

         cc.cvtss2sd(y, y);
         cc.cvtss2sd(x, x);

         InvokeNode *toPow;
         cc.invoke(&toPow,
                   (double (*)(double, double))&pow,
                   FuncSignatureT<double, double, double>());
         toPow->setArg(0, y);
         toPow->setArg(1, x);
         toPow->setRet(0, y);

         cc.cvtsd2ss(y, y);
         cc.movss(x, top2);
      }

      break;
   default:
      throw NotYetImplemented();
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
             FuncSignatureT<float, float>());

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

