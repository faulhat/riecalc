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
             *grid,
             *expr_entry,
             *go_button,
             *graphing_area,
             *xmin_entry,
             *xmax_entry,
             *ymin_entry,
             *ymax_entry,
             *err_area;

   double xmin, xmax,
          ymin, ymax;
   
   /* Objects used for the compilation of expressions. */
   const ExecCtx ectx;
   JitRuntime rt;
   Func fn;

   /* Sets fn to the expression in the given expr object. */
   void apply_expr(const Expr *expr);

   /* Parses a string with an expression and sets fn.
    * Adds an error message to the dialog if the parser produced an error.
    */
   void apply_fn_str(const char *in);
public:
   /* Displays the window and the graph. */
   void activate_graph();
   
   /* Reloads expression from entry area */
   void reload_expr();

   /* Runs the graphing program */
   int run(int argc, char **argv);

   /* Redraws the graph */
   gboolean draw_graph(cairo_t *cr);
};

#endif

