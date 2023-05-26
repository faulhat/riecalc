#include "asmjit/src/asmjit/asmjit.h"
#include <stdio.h>

using namespace asmjit;

// Signature of the generated function.
typedef float (*Func)(float, float);

int main(int argc, char* argv[]) {
  JitRuntime rt;
  CodeHolder code;
  code.init(rt.environment(), rt.cpuFeatures());
  x86::Assembler a(&code);

  a.addss(x86::xmm0, x86::xmm1);
  a.ret();

  Func fn;
  Error err = rt.add(&fn, &code);
  if (err) {
    printf("AsmJit failed: %s\n", DebugUtils::errorAsString(err));
    return 1;
  }

  float result = fn(1.0, 2.1);
  printf("%f\n", result);

  rt.release(fn);
  return 0;
}

