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

#include "grapher.hpp"

#include <sstream>
#include <cstring>

int run_tests();

void repl();

int main(int argc, char **argv) {
   static const char *name = "Riemann";

   if (argc > 1 && (strncmp(argv[1], "-t", 2) == 0 ||
                    strcmp(argv[1], "--test") == 0)) {
      return run_tests();
   } else if (argc > 1 && (strncmp(argv[1], "-g", 2) == 0 ||
                           strcmp(argv[1], "--graph") == 0)) {
      char *g_argv[] = { strdup(name) };
      int status = Grapher().run(1, g_argv);

      free(g_argv[0]);
      return status;
   } else {
      repl();
      return 0;
   }
}
