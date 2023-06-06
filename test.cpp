extern "C" {
   #include "expr.h"
}

#include "compile.hpp"
#include <vector>
#include <cassert>

void test_expr(JitRuntime &rt,
               const char *in,
               Table &table,
               int *ctr, int *fails,
               double expected = 0,
               double delta = 0.001) {
   Expr *expr = nullptr;
   double result;
   char *funcname;
   try {
      if (conv_eval_str(rt, in, table, &expr, &result, &funcname)) {
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
      else {
         printf("%s(X) = ", funcname);
         print_expr(expr, (FILE *)stdout);
         printf("\n\n");
         free(funcname);
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

   static const char *functions[] = {
      "F = 2x + 1",
      "G = 2^xF(x)",
      "E = 2.71828",
      "Ln = Log(x)/Log(E())",
      "Pi = 3.1415926535"
   };

   Table table;
   int ctr = 0, fails = 0;
   for (int i = 0; i < 5; i++) {
      test_expr(rt, functions[i], table, &ctr, &fails); 
   }
   
   test_expr(rt, "F(3)", table, &ctr, &fails, 7);
   test_expr(rt, "G(2)", table, &ctr, &fails, 20);
   test_expr(rt, "G(3)^-F(-1)", table, &ctr, &fails, 56);
   test_expr(rt, "E()^(Ln(5) + Ln(2))", table, &ctr, &fails, 10);
   test_expr(rt, "Cos(Pi())", table, &ctr, &fails, -1);
   test_expr(rt, "2[Sin(3 * Pi()/2)]", table, &ctr, &fails, 2);
   printf("%d tests completed. %d failures. %d successes.\n", ctr, fails, ctr - fails);

   for (auto f: table) {
      rt.release(*f.second);
   }
}
 
