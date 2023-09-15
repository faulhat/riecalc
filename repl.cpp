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

#include "compile.hpp"

#include <cstdio>
#include <iostream>
#include <sstream>

using namespace asmjit;

/**
 * Runs the repl program in the console, allowing the user to input definitions and expressions to evaluate.
 */
void repl() {
   JitRuntime rt;
 
   Expr *expr;
   std::stringstream linestream;
   std::string line;
   char next;

   ExecCtx ectx;
   double result;
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

      expr = nullptr;
      try {
         if (conv_eval_str(rt, line.c_str(), ectx, &expr, result)) {
            printf("> ");
            print_expr(expr, (FILE *)stdout);
            printf(" = %.2f\n", result);
         }

         printf("\n");
      }
      catch (ReportingException *e) {
         e->report();
      }

      if (expr != nullptr) {
        destroy_expr(expr);
      }
   }

   for (auto f: ectx.fnTable) {
      rt.release(*f.second);
   }
}

