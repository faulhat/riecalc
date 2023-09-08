#ifndef GRAPHER_HPP
#define GRAPHER_HPP

#include "compile.hpp"
#include <gtk/gtk.h>

/* A class representing the graphing dialog. */
class Grapher {
   /* The underlying Gtk object */
   GtkApplication *app;

   /* The components of the grapher */
   GtkWidget  *grid,
             *expr_entry,
             *graphing_area,
             *xmin_entry,
             *xmax_entry,
             *ymin_entry,
             *ymax_entry,
             *err_area;

   /* Graph window settings */
   double xmin, xmax,
          ymin, ymax;

   /* Components of the analysis menu */
   GtkWidget *rs_lower_entry,
             *rs_upper_entry,
             *rs_step_entry;

   /* Riemann summation input data */
   double rs_lower, rs_upper, rs_step;
   
   /* Objects used for the compilation of expressions. */
   const ExecCtx ectx;
   JitRuntime rt;
   Func fn;

   /* Flag telling draw function to incorporate riemann sum */
   bool do_rs;

   /* Loads riemann sum vars */
   bool load_rs_vars();

   /* Sets fn to the expression in the given expr object. */
   void apply_expr(const Expr *expr);

   /* Parses a string with an expression and sets fn.
    * Adds an error message to the dialog if the parser produced an error.
    */
   void apply_fn_str(const char *in);

   /* Constructs the graphing area */
   void make_grapher();

   /* Constructs the window settings area */
   void make_settings();

   /* Constructs the analysis menu */
   void make_analysis();

public:
   /* Constructs the full UI */
   void make_all();
   
   /* Reloads expression from entry area */
   void reload_expr(bool rs);

   /* Runs the graphing program */
   int run(int argc, char **argv);

   /* Redraws the graph */
   gboolean draw_graph(cairo_t *cr);

};

#endif

