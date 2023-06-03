#include <sstream>

void run_tests();

void repl();
   
int main(int argc, char **argv) {
   if (argc > 1 && argv[1][0] == 't') {
      run_tests();
   }
   else {
      repl();
   }
   
   return 0;
}
