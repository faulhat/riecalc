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

#ifndef GRAPHER_HPP
#define GRAPHER_HPP

#include "compile.hpp"
#include <gtk/gtk.h>

/* A class representing the graphing dialog. */
class Grapher {
   /* The underlying Gtk object */
   GtkApplication *app;

   /* The components of the grapher */
   GtkWidget *grid,
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

   /* Components of the trace menu */
   GtkWidget *tr_xval_entry,
             *tr_res_area;

   /* x value for trace */
   double tr_xval;

   /* Components of the analysis menu */
   GtkWidget *rs_lower_entry,
             *rs_upper_entry,
             *rs_step_entry,
             *rs_res_area;

   /* Riemann summation input data */
   double rs_lower, rs_upper, rs_step;
   
   /* Objects used for the compilation of expressions. */
   const ExecCtx ectx;
   JitRuntime rt;
   Func fn;

   /* Flag telling draw function to incorporate trace */
   bool do_tr;

   /* Flag telling draw function to incorporate riemann sum */
   bool do_rs;

   /**
    * Loads x value for trace function
    *
    * @return true if there were no errors
    */
   bool load_xval();

   /**
    * Loads riemann sum vars
    *
    * @return true if there were no errors
    */
   bool load_rs_vars();

   /**
    * Sets the function being graphed
    *
    * @param expr The new expression
    */
   void apply_expr(const Expr *expr);

   /**
    * Parses an expression string and graphs the expression
    *
    * @param in The input string
    */
   void apply_fn_str(const char *in);

   /**
    * Constructs the graphing area
    */
   void make_grapher();

   /**
    * Constructs the window settings area
    */
   void make_settings();

   /**
    * Constructs the analysis menu 
    */
   void make_analysis();

public:
   /**
    * Constructs the full grapher UI 
    */
   void make_all();
   
   /**
    * Reloads expression from entry area
    *
    * @param trace Whether or not to apply the trace function
    * @param rsum Whether or not to take a Riemann sum
    */
   void reload_expr(bool trace, bool rsum);

   /**
    * Runs the graphing program
    *
    * @param argc Command line arg count, passed to GTK
    * @param argv Command line args, passed to GTK
    *
    * @return GTK status
    */
   int run(int argc, char **argv);

   /**
    * Redraws the graph
    *
    * @param cr The cairo surface supplied by GTK
    *
    * @return FALSE
    */
   gboolean draw_graph(cairo_t *cr);
};

#endif

