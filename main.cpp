#include "grapher.hpp"

#include <sstream>
#include <cstring>

int run_tests();

void repl();

int main(int argc, char **argv) {
   if (argc > 1 && (strncmp(argv[1], "-t", 2) == 0 ||
                    strcmp(argv[1], "--test") == 0)) {
      return run_tests();
   } else if (argc > 1 && (strncmp(argv[1], "-g", 2) == 0 ||
                           strcmp(argv[1], "--graph") == 0)) {
      return Grapher().run(argc - 1, argv + 1);
   } else {
      repl();
      return 0;
   }
}
