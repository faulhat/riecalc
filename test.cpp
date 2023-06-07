extern "C" {
   #include "expr.h"
}

#include "compile.hpp"
#include <vector>
#include <cassert>

void test_expr(JitRuntime &rt,
               const char *in,
               ExecCtx &ectx,
               int *ctr, int *fails,
               double expected = 0,
               double delta = 0.001) {
   Expr *expr = nullptr;
   double result;
   char *funcname = nullptr, *varname = nullptr;
   try {
      if (conv_eval_str(rt, in, ectx, &expr, result, &funcname, &varname)) {
         printf("> ");
         print_expr(expr, (FILE *)stdout);
         printf(" = %.4f\n", result);
         if (result < expected - delta || result > expected + delta) {
            printf("FAILED! Expected: %f\n\n", expected);
            ++*fails;
         }
         else {
            printf("Success!\n\n");
         }

         ++*ctr;
      }
      else if (funcname != nullptr) {
         printf("%s(x) = ", funcname);
         print_expr(expr, (FILE *)stdout);
         printf("\n\n");
         free(funcname);
      }
      else if (varname != nullptr) {
         printf("%s = ", varname);
         print_expr(expr, (FILE *)stdout);
         printf("\n\n");
         free(varname);
      }
   }
   catch (ReportingException *e) {
      e->report();
   }

   destroy_expr(expr);
}

void run_tests() {
   JitRuntime rt;
   Expr *expr;
   char *funcname;

   std::vector<const char *> functions = {
      "F = 2x + 1",
      "G = 2^x * F(x)",
      "Ln = Log(x)/Log(e)",
   };

   ExecCtx ectx;
   int ctr = 0, fails = 0;
   for (auto fn: functions) {
      test_expr(rt, fn, ectx, &ctr, &fails); 
   }
   
   test_expr(rt, "F(3)", ectx, &ctr, &fails, 7);
   test_expr(rt, "G(2)", ectx, &ctr, &fails, 20);
   test_expr(rt, "G(3)^-F(-1)", ectx, &ctr, &fails, 56);
   test_expr(rt, "e^(Ln(5) + Ln(2))", ectx, &ctr, &fails, 10);
   test_expr(rt, "Cos(pi)", ectx, &ctr, &fails, -1);
   test_expr(rt, "2[Sin(3 * pi/2)]", ectx, &ctr, &fails, 2);
   printf("%d tests completed. %d failures. %d successes.\n", ctr, fails, ctr - fails);

   for (auto f: ectx.fnTable) {
      rt.release(*f.second);
   }
}
 
