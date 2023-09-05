#ifndef GRAPHER_HPP
#define GRAPHER_HPP

#include "compile.hpp"
#include <gtk/gtk.h>

/* A class representing the graphing dialog. */
class Grapher {
   /* The underlying Gtk object */
   GtkApplication *app;

   /* The components of the grapher */
   GtkWidget *window,
             *graphing_area;
   
   /* Objects used for the compilation of expressions. */
   const ExecCtx ectx;
   JitRuntime rt;
   Func fn;

   /* Sets fn to the expression in the given expr object. */
   void apply_expr(const Expr *expr);

public:
   /* Sets up window and compilation context. */
   Grapher();

   /* Parses a string with an expression and sets fn.
    * Adds an error message to the dialog if the parser produced an error.
    */
   void apply_fn_str(const char *in);

   /* Displays the window and the graph. */
   void activate_graph();

   /* Runs the graphing program */
   int run(int argc, char **argv);

   /* Getter for the function field */
   const Func &getFn() const;
};

#endif

