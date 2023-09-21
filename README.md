# riemann

`riemann` is a graphing calculator program for Linux that includes built-in integral approximation via Riemann sums and Monte Carlo sampling, with more analysis capabilities planned to be added in the future. In order to make these processes fast, `riemann` just-in-time compiles provided expressions into real x86 subroutines using the [asmjit](https://github.com/asmjit/asmjit/) library.

## Build instructions

To build `riemann` on your local system, just do:
```
git clone https://github.com/faulhat/riemann
cd riemann
make
```

Then, to run the graphing calculator:
```
./rcalc -g
```

To instead run a simple repl version of the calculator, exclude the `-g` flag. To run the test suite, use the `-t` flag.

## Expression notation

Expressions used as input to the graphing calculator can incorporate some basic functions, such as sine and cosine. These functions must be written out with a capital letter, e.g. `Sin(x)` and `Cos(x)`.

Expressions may also incorporate concise multiplication notation in place of the `*` operator, e.g. `3x` or `xSin(x)`.

Square brackets may be used in place of parentheses to indicate absolute value, e.g. `[x^3]/2` or `Sqrt[x]`.

The repl allows you to define your own functions, which must also start with a capital letter. An example would be:
```
F = 3x + 1

F(9)
> F(9.00) = 28.00
```

`e` and `pi` can also be used as built-in constants.
```
Sin(pi) + Log(e)
> (Sin(3.14) + Log(2.72)) = 1.00
```

## Design choices

This project builds `asmjit` from source instead of having it as a library dependency. It also uses the C API for GTK+ 3 instead of the much simpler and more concise C++ version, `gtkmm`. Both of these decisions were made so that I could run this program on the WWU CS department computers, on which I can't use `apt` and am therefore limited to what they already have on the system.

