#include "compile.hpp"

#include <cstdio>
#include <iostream>
#include <sstream>

using namespace asmjit;

int main() {
   JitRuntime rt;
   
   Expr *expr;
   std::stringstream linestream;
   std::string line;
   char next;

   Table table;
   add_default_fns(table);

   Func fn;
   char *funcname;
   int errc;
   const char **errv;
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
      bool input_null = true;
      for (char ch: line) {
         if (!isspace(ch)) {
            input_null = false;
            break;
         }
      }

      if (input_null) {
         continue;
      }

      yy_scan_string(line.c_str());
      
      funcname = nullptr;
      errc = 0;
      yyparse(&expr, &funcname, &errc, &errv);
      if (errc != 0) {
         for (int i = 0; i < errc; i++) {
            printf("Parser error: %s\n", errv[i]);
         }
         
         printf("\n");
         free(errv);
         continue;
      }

      try {
         fn = conv_var_expr(expr, rt, &table);
      }
      catch (NameResFail err) {
         err.report();
         continue;
      }

      if (funcname != nullptr) {
         table[std::string(funcname)] = fn;
         free(funcname);
         printf("\n");
      }
      else {
         double result = (*fn)(0);
         printf("\n> ");
         print_expr(expr, (FILE *)stdout);
         printf(" = %.4f\n\n", result);
      }     
 
      yylex_destroy();
      destroy_expr(expr);
   }

   for (auto f: table) {
      rt.release(*f.second);
   }

   return 0;
}
