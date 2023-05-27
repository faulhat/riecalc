extern "C" {
   #include "expr.h"
   #include "lexer.h"
   #include "parser.h"
}

#include "asmjit/src/asmjit/x86.h"
#include <cmath>
#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>

using namespace asmjit;

typedef float (*Func)(float);

void _conv_var_expr_rec(const Expr *expr,
                        x86::Compiler &cc,
                        x86::Xmm &y,
                        x86::Xmm &x,
                        x86::Gp &i) {
   x86::Mem top = cc.newStack(4, 16);
   top.setSize(4);
   switch (expr->type) {
   case UNARY:
      switch (expr->val.unary->op) {
      case NEG:
         _conv_var_expr_rec(expr->val.unary->inner, cc, y, x, i);
         cc.movss(top, y);
         cc.xorps(y, y);
         cc.subss(y, top);
         break;
      case ABS:
         _conv_var_expr_rec(expr->val.unary->inner, cc, y, x, i);
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
      
      break;
   case BINARY:
      _conv_var_expr_rec(expr->val.binary->rhs, cc, y, x, i);
      cc.movss(top, y);
      _conv_var_expr_rec(expr->val.binary->lhs, cc, y, x, i);
      switch (expr->val.binary->op) {
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
      default:
         fprintf(stderr, "Not yet implemented!\n");
         exit(-1);
      }
      
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

void conv_var_expr(const Expr *expr,
                   JitRuntime &rt,
                   Func &fn) {
   CodeHolder code;
   code.init(rt.environment(), rt.cpuFeatures());
   x86::Compiler cc(&code);

   FuncNode *f = cc.addFunc(FuncSignatureT<float, float>());
   x86::Xmm y = cc.newXmm();
   x86::Xmm x = cc.newXmm();
   x86::Gp i = cc.newGpd();
   f->setArg(0, x);

   _conv_var_expr_rec(expr, cc, y, x, i);
   
   cc.ret(y);
   cc.endFunc();
   cc.finalize();
   
   Error err = rt.add(&fn, &code);
   if (err) {
      printf("AsmJit failed: %s\n", DebugUtils::errorAsString(err));
      exit(1);
   }
}

int main() {
   JitRuntime rt;
   
   Func fn;
   Expr *expr;
   std::stringstream linestream;
   std::string line;
   char next;

   while (true) {
      linestream.str("");

      do {
         next = getchar();
         if (next == EOF) {
            exit(0);
         }

         linestream << next;
      } while (next != '\n' && next != EOF);

      line = linestream.str();
      yy_scan_string(line.c_str());
      
      yyparse(&expr);
      conv_var_expr(expr, rt, fn);

      float result = fn(3.0);
      printf("When X = 3:\n");
      print_expr(expr, (FILE *)stdout);
      printf(" = %.4f\n\n", result);
      
      rt.release(fn);
   }

   return 0;
}
