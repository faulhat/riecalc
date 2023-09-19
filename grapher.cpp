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
#include "drawing.hpp"

#include <cmath>

/**
 * "draw" callback for the graphing area.
 * Just calls the Grapher class's internal method,
 * since it needs access to the other widgets of the display
 *
 * @param widget Should always be the main graphing area
 * @param cr The cairo surface supplied by GTK
 * @param data A pointer to the underlying Grapher object
 *
 * @return FALSE
 */
gboolean draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
   return ((Grapher *)data)->draw_graph(cr);
}

gboolean Grapher::draw_graph(cairo_t *cr) {
   static const GdkRGBA BLACK = {0.0, 0.0, 0.0, 1.0},
                        FOG = {0.3, 0.3, 0.3, 1.0},
			               GREEN = {
                            57.0 / 255,
                           255.0 / 255,
                            20.0 / 255, 1.0},
                        WHITE = {1.0, 1.0, 1.0, 1.0},
                        RED = {1.0, 0.0, 0.0, 1.0},
                        RED_HALF = {1.0, 0.0, 0.0, 0.5},
                        BLUE_HALF = {0.0, 0.0, 1.0, 0.5};

   GtkStyleContext *ctx = gtk_widget_get_style_context(graphing_area);
   guint width = gtk_widget_get_allocated_width(graphing_area),
         height = gtk_widget_get_allocated_height(graphing_area);

   gtk_render_background(ctx, cr, 0, 0, width, height);

   gdk_cairo_set_source_rgba(cr, &BLACK);
   cairo_rectangle(cr, 0, 0, width, height);
   cairo_fill(cr);

   double xrange = xmax - xmin,
          yrange = ymax - ymin;

   gdk_cairo_set_source_rgba(cr, &FOG);
   cairo_set_line_width(cr, 1);

   // The guideline grid should always have a reasonable number of lines onscreen.
   // That's what this calculation and the equivalent below for the y-axis are for.
   // There ought to be an average of 10 lines along either axis.
   double xlog = std::floor(std::log(xrange) / std::log(10));
   if (xrange / std::pow(10, xlog) < 5.0) {
      xlog -= 1.0;
   }

   double xstep = std::pow(10, xlog);
   for (double x = xstep * std::ceil(xmin / xstep);
        x < xmax;
        x += xstep) {
      double x_line = width * (x - xmin) / xrange;
      cairo_move_to(cr, x_line, 0);
      cairo_line_to(cr, x_line, height);
      cairo_stroke(cr);
   }

   double ylog = std::floor(std::log(yrange) / std::log(10));
   if (yrange / std::pow(10, ylog) < 5.0) {
      ylog -= 1.0;
   }

   double ystep = std::pow(10, ylog);
   for (double y = ystep * std::ceil(ymin / ystep);
        y < ymax;
        y += ystep) {
      double y_line = height * (1 - ((y - ymin) / yrange));
      cairo_move_to(cr, 0, y_line);
      cairo_line_to(cr, width, y_line);
      cairo_stroke(cr);
   }

   gdk_cairo_set_source_rgba(cr, &WHITE);
   double y_zero_line = height * (1 - (-ymin / yrange));
   if (y_zero_line > 0 && y_zero_line < height) {
      cairo_move_to(cr, 0, y_zero_line);
      cairo_line_to(cr, width, y_zero_line);
      cairo_stroke(cr);
   }
   
   double x_zero_line = width * -xmin / xrange;
   if (x_zero_line > 0 && x_zero_line < width) {
      cairo_move_to(cr, x_zero_line, 0);
      cairo_line_to(cr, x_zero_line, height);
      cairo_stroke(cr);
   }


   if (fn != nullptr) {
      if (do_rs) {
         double sum = 0;
         int last_x_line = (int) std::floor(width * (rs_lower - rs_step - xmin) / xrange);
         int step_width = (int) std::ceil(width * rs_step / xrange);
         for (double x = rs_lower;
              x < rs_upper;
              x += rs_step) {
            double y = fn(x + rs_step / 2.0);
            // NaNs are skipped in both the calculation of the sum and the drawing.
            if (!std::isnan(y)) {
               sum += rs_step * y;
            }

            // Drawing a rectangle for every point gets too expensive at low step sizes.
            // This makes sure only 1 rectangle is drawn per pixel wide.
            int x_lo_line = (int) std::floor(width * (x - xmin) / xrange);
            if (x_lo_line - last_x_line >= step_width) {
               if (!std::isnan(y)) {
                  double y_line = height * (1 - (y - ymin) / yrange);
                  if (y_line < 0) {
                     y_line = 0;
                  } else if (y_line > height) {
                     y_line = height;
                  }

                  if (y < 0) {
                     gdk_cairo_set_source_rgba(cr, &BLUE_HALF);
                     cairo_rectangle(cr,
                                     last_x_line + step_width,
                                     y_zero_line,
                                     x_lo_line - last_x_line,
                                     y_line - y_zero_line);
                  } else {
                     gdk_cairo_set_source_rgba(cr, &RED_HALF);
                     cairo_rectangle(cr,
                                     last_x_line + step_width,
                                     y_line,
                                     x_lo_line - last_x_line,
                                     y_zero_line - y_line);
                  }

                  cairo_fill(cr);
               }

               last_x_line = x_lo_line;
            }
         }
            
         std::string res = std::to_string(sum);
         gtk_label_set_text(GTK_LABEL(rs_res_area), res.c_str());
      }

      gdk_cairo_set_source_rgba(cr, &GREEN);
      bool offscreen = false;
      Point B = Point::of(fn, xmin);
      Point A;
      for (guint i = 0; i < width; i++) {
         // x := x(i) + 1px delta
         double x = ((double)(i + 1) / width) * xrange + xmin;
         
         A = B;
         B = Point::of(fn, x);

         if (std::isnan(A.y) || std::isnan(B.y)) {
            cairo_stroke(cr);
            continue;
         }

         Point mid = midpoint(A, B);
         if (std::isinf(mid.y)) {
            if (!offscreen) {
               if (mid.y > 0) {
                  cairo_line_to(cr, width * (x - xmin) / xrange, 0);
               } else {
                  cairo_line_to(cr, width * (x - xmin) / xrange, height);
               }

               cairo_stroke(cr);
               offscreen = true;
            } else if (mid.y > 0) {
               cairo_move_to(cr, width * (x - xmin) / xrange, 0);
            } else {
               cairo_move_to(cr, width * (x - xmin) / xrange, height);
            }

            continue;
         }

         Point pos_inf = goes_pos_inf(fn, A, B, xrange);
         Point neg_inf = { 0, 0 };
         if (pos_inf.y != 0) {
            // Check if it goes to infinity from the left or the right.
            // Assumes a continuous ascent from whichever side
            double lhs_mid = fn((A.x + pos_inf.x) / 2.0);
            if (lhs_mid > A.y) {
               // We assume it comes from the left
               cairo_line_to(cr, width * (pos_inf.x - xmin) / xrange, 0);

               // What's on the other side?
               neg_inf = goes_neg_inf(fn, pos_inf, B, xrange);
               if (neg_inf.y != 0) {
                  cairo_stroke(cr);
                  cairo_move_to(cr, width * (neg_inf.x - xmin) / xrange, height);
               } else {
                  pos_inf = goes_pos_inf(fn, pos_inf, B, xrange);
                  if (pos_inf.y == 0) {
                     // It doesn't go to infinity and come back.
                     // There's some other kind of jump. 
                     cairo_stroke(cr);
                  }
               }
            } else {
               // We assume it comes from the right.
               // But then what happens on the left side?
               neg_inf = goes_neg_inf(fn, A, pos_inf, xrange);
               if (neg_inf.y != 0) {
                  cairo_line_to(cr, width * (neg_inf.x - xmin) / xrange, height);
                  cairo_stroke(cr);
               } else {
                  Point lhs_pos_inf = goes_pos_inf(fn, A, pos_inf, xrange);
                  if (lhs_pos_inf.y == 0) {
                     cairo_stroke(cr);
                  }
               }

               cairo_move_to(cr, width * (pos_inf.x - xmin) / xrange, 0);
            }
         } else {
            // The mirror image of the above.
            neg_inf = goes_neg_inf(fn, A, B, xrange);
            if (neg_inf.y != 0) {
               double lhs_mid = fn((A.x + neg_inf.x) / 2.0);
               if (lhs_mid < A.y) {
                  cairo_line_to(cr, width * (neg_inf.x - xmin) / xrange, height);

                  pos_inf = goes_pos_inf(fn, neg_inf, B, xrange);
                  if (pos_inf.y != 0) {
                     cairo_stroke(cr);
                     cairo_move_to(cr, width * (pos_inf.x - xmin) / xrange, 0);
                  } else {
                     neg_inf = goes_neg_inf(fn, neg_inf, B, xrange);
                     if (neg_inf.y == 0) {
                        cairo_stroke(cr);
                     }
                  }
               } else {
                  pos_inf = goes_pos_inf(fn, neg_inf, A, xrange);
                  if (pos_inf.y != 0) {
                     cairo_line_to(cr, width * (pos_inf.x - xmin) / xrange, 0);
                     cairo_stroke(cr);
                  } else {
                     Point lhs_neg_inf = goes_neg_inf(fn, neg_inf, A, xrange);
                     if (lhs_neg_inf.y == 0) {
                        cairo_stroke(cr);
                     }
                  }

                  cairo_move_to(cr, width * (neg_inf.x - xmin) / xrange, 0);
               }
            }
         }

         if (pos_inf.y == 0 && neg_inf.y == 0) {
            // No discontinuity. We draw the line through the midpoint.
            Point next = midpoint(A, B).scale(xmin, xmax, width,
                                              ymin, ymax, height);
            
            if (next.y < 0 || next.y > height) {
               if (!offscreen) {
                  cairo_line_to(cr, next.x, next.y);
                  offscreen = true;
               } else {
                  cairo_move_to(cr, next.x, next.y);
               }
            } else {
               cairo_line_to(cr, next.x, next.y);
               offscreen = false;
            }
         }
      }

      cairo_stroke(cr);

      if (do_tr) {
         gdk_cairo_set_source_rgba(cr, &RED);
         cairo_set_line_width(cr, 2);

         int i = (int)(width * (tr_xval - xmin) / xrange);
         double y = fn(tr_xval);
         if (!std::isnan(y) && !std::isinf(y)) {
            int j = (int)(height * (1 - (y - ymin) / yrange));
            cairo_arc(cr, i, j, 5, 0, 2 * G_PI);
            cairo_stroke(cr);
         }

         std::string res = std::to_string(y);
         gtk_label_set_text(GTK_LABEL(tr_res_area), res.c_str());
      }
   }

   return FALSE;
}

/**
 * Attempts to get a double from a GtkEntry widget
 * Fills in an error field if it failed
 *
 * @param entry The widget to take input from
 * @param into Where to return the value
 * @param err_label Where to write the error message to
 * @param err_msg The message to write
 *
 * @return true if no error was produced.
 */
bool get_double_from_gtk_entry(GtkWidget *entry,
                               double *into,
                               GtkWidget *err_label,
                               const char *err_msg) {
   static const char *fmt = "%lf%c";
   char extra;
   int tokens = sscanf(gtk_entry_get_text(GTK_ENTRY(entry)),
                                          fmt, into, &extra);
   
   bool failed = tokens != 1;
   if (failed) {
      gtk_label_set_text(GTK_LABEL(err_label), err_msg);
   }

   return !failed;
}

bool Grapher::load_xval() {
   return get_double_from_gtk_entry(
            tr_xval_entry,
            &tr_xval,
            err_area,
            "Error: could not parse x value for trace.");
}

bool Grapher::load_rs_vars() {
   // Takes advantage of short-circuiting
   bool success =
         get_double_from_gtk_entry(
            rs_lower_entry,
            &rs_lower,
            err_area,
            "Error: could not parse lower integration bound.")
      && get_double_from_gtk_entry(
            rs_upper_entry,
            &rs_upper,
            err_area,
            "Error: could not parse upper integration bound.")
      && get_double_from_gtk_entry(
            rs_step_entry,
            &rs_step,
            err_area,
            "Error: could not parse integration step size.");
   
   if (success) {
      if (rs_upper < rs_lower) {
         gtk_label_set_text(GTK_LABEL(err_area),
                            "Error: flipped integration bounds.");
         success = false;
      } else if (rs_step < 1e-7) {
         gtk_label_set_text(GTK_LABEL(err_area),
                            "Error: step size too small.");
         success = false;
      }
   }

   return success;
}

bool Grapher::load_mc_vars() {
   bool success =
         get_double_from_gtk_entry(
            mc_xmin_entry,
            &mc_xmin,
            err_area,
            "Error: could not parse left bound for approx. area.")
      && get_double_from_gtk_entry(
            mc_xmax_entry,
            &mc_xmax,
            err_area,
            "Error: could not parse right bound for approx. area.")
      && get_double_from_gtk_entry(
            mc_ymin_entry,
            &mc_ymin,
            err_area,
            "Error: could not parse bottom bound for approx. area.")
      && get_double_from_gtk_entry(
            mc_ymax_entry,
            &mc_ymax,
            err_area,
            "Error: could not parse top bound for approx. area.");

   if (success) {
      if (mc_xmax < mc_xmin) {
         gtk_label_set_text(GTK_LABEL(err_area),
                           "Error: flipped horizontal area bounds.");
         success = false;
      } else if (mc_ymax < mc_ymin) {
         gtk_label_set_text(GTK_LABEL(err_area),
                           "Error: flipped vertical area bounds.");
         success = false;
      }
   }
   
   return success;
}

/**
 * "activate" callback for the entire grapher.
 *
 * @param app The underlying application
 * @param data A pointer to the Grapher object itself
 */
void activate(GtkApplication *app, gpointer data) {
   ((Grapher *)data)->make_all();
}

void Grapher::apply_expr(const Expr *expr) {
   fn = conv_expr(expr, rt, ectx);
}

void Grapher::apply_fn_str(const char *in) {
   Expr *expr = nullptr;

   char *funcname = nullptr,
        *varname = nullptr;

   char *err = nullptr;

   yy_scan_string(in);
   yyparse(&expr, &funcname, &varname, &err);
   
   if (err != nullptr) {
      gtk_label_set_text(GTK_LABEL(err_area), err);
      return;
   }

   apply_expr(expr);

   if (funcname != nullptr) {
      free(funcname);
   }

   if (varname != nullptr) {
      free(varname);
   }
   yylex_destroy();
}

/**
 * Callback to reload expression and redraw graph
 *
 * @param widget The caller
 * @param data The Grapher object
 */
void load_expr(GtkWidget *widget, gpointer data) {
   ((Grapher *)data)->reload_expr(false, false);
}

void Grapher::reload_expr(bool trace, bool rsum) {
   do_tr = trace;
   do_rs = rsum;

   if (!do_tr) {
      gtk_label_set_text(GTK_LABEL(tr_res_area), "");
   }

   if (!do_rs) {
      gtk_label_set_text(GTK_LABEL(rs_res_area), "");
   }

   gtk_label_set_text(GTK_LABEL(err_area), "");
   bool all_parsed =
      get_double_from_gtk_entry(
         xmin_entry,
         &xmin,
         err_area,
         "Error: could not parse xmin.")
   && get_double_from_gtk_entry(
         xmax_entry,
         &xmax,
         err_area,
         "Error: could not parse xmax.")
   && get_double_from_gtk_entry(
         ymin_entry,
         &ymin,
         err_area,
         "Error: could not parse ymin.")
   && get_double_from_gtk_entry(
         ymax_entry,
         &ymax,
         err_area,
         "Error: could not parse ymax.");

   if (all_parsed) {
      if (do_rs) {
         // Only include riemann sum if parsing those vars succeeds
         do_rs = load_rs_vars();
      } else if (do_tr) {
         do_tr = load_xval();
      }

      try {
         const char *expr_str = gtk_entry_get_text(GTK_ENTRY(expr_entry));
         apply_fn_str(expr_str);

         gtk_widget_queue_draw(graphing_area);
      } catch (NameResFail *e) {
         gtk_label_set_text(GTK_LABEL(err_area), e->what());
         delete e;
      } catch (ParseError *e) {
         gtk_label_set_text(GTK_LABEL(err_area), e->what());
         delete e;
      }
   }
}

void Grapher::make_grapher() {
   GtkWidget *expr_label = gtk_label_new("Enter an expression in terms of x:");
   gtk_widget_set_halign(expr_label, GTK_ALIGN_START);
   gtk_grid_attach(GTK_GRID(grid), expr_label, 0, 0, 7, 1);

   expr_entry = gtk_entry_new();
   gtk_grid_attach(GTK_GRID(grid), expr_entry, 0, 1, 4, 1);
   g_signal_connect(G_OBJECT(expr_entry), "activate",
                    G_CALLBACK(load_expr), this);

   GtkWidget *go_button = gtk_button_new_with_label("Go");
   gtk_grid_attach(GTK_GRID(grid), go_button, 4, 1, 1, 1);
   g_signal_connect(G_OBJECT(go_button), "clicked",
                    G_CALLBACK(load_expr), this);

   graphing_area = gtk_drawing_area_new();
   gtk_widget_set_size_request(graphing_area, 650, 400);
   gtk_widget_set_vexpand(graphing_area, TRUE);
   gtk_widget_set_hexpand(graphing_area, TRUE);
   gtk_widget_set_valign(graphing_area, GTK_ALIGN_CENTER);
   gtk_widget_set_halign(graphing_area, GTK_ALIGN_CENTER);

   gtk_grid_attach(GTK_GRID(grid), graphing_area, 0, 2, 5, 5);
   g_signal_connect(G_OBJECT(graphing_area), "draw",
                    G_CALLBACK(draw), this);
}

void Grapher::make_settings() {
   GtkWidget *xmin_label = gtk_label_new("xmin");
   gtk_grid_attach(GTK_GRID(grid), xmin_label, 1, 7, 1, 1);

   GtkWidget *xmax_label = gtk_label_new("xmax");
   gtk_grid_attach(GTK_GRID(grid), xmax_label, 2, 7, 1, 1);

   GtkWidget *ymin_label = gtk_label_new("ymin");
   gtk_grid_attach(GTK_GRID(grid), ymin_label, 3, 7, 1, 1);

   GtkWidget *ymax_label = gtk_label_new("ymax");
   gtk_grid_attach(GTK_GRID(grid), ymax_label, 4, 7, 1, 1);

   GtkWidget *dim_label = gtk_label_new("Window:");
   gtk_grid_attach(GTK_GRID(grid), dim_label, 0, 8, 1, 1);

   xmin_entry = gtk_entry_new();
   gtk_entry_set_text(GTK_ENTRY(xmin_entry), "-10.0");
   gtk_grid_attach(GTK_GRID(grid), xmin_entry, 1, 8, 1, 1);
   g_signal_connect(G_OBJECT(xmin_entry), "activate",
                    G_CALLBACK(load_expr), this);
   xmin = -10.0;

   xmax_entry = gtk_entry_new();
   gtk_entry_set_text(GTK_ENTRY(xmax_entry), "10.0");
   gtk_grid_attach(GTK_GRID(grid), xmax_entry, 2, 8, 1, 1);
   g_signal_connect(G_OBJECT(xmax_entry), "activate",
                    G_CALLBACK(load_expr), this);
   xmax = 10.0;

   ymin_entry = gtk_entry_new();
   gtk_entry_set_text(GTK_ENTRY(ymin_entry), "-10.0");
   gtk_grid_attach(GTK_GRID(grid), ymin_entry, 3, 8, 1, 1);
   g_signal_connect(G_OBJECT(ymin_entry), "activate",
                    G_CALLBACK(load_expr), this);
   ymin = -10.0;

   ymax_entry = gtk_entry_new();
   gtk_entry_set_text(GTK_ENTRY(ymax_entry), "10.0");
   gtk_grid_attach(GTK_GRID(grid), ymax_entry, 4, 8, 1, 1);
   g_signal_connect(G_OBJECT(ymax_entry), "activate",
                    G_CALLBACK(load_expr), this);
   ymax = 10.0;

   err_area = gtk_label_new("");
   gtk_grid_attach(GTK_GRID(grid), err_area, 0, 9, 5, 1);
}

/**
 * Callback to reload expression and redraw graph with trace.
 *
 * @param widget The caller
 * @param data The Grapher object
 */
void load_expr_tr(GtkWidget *widget, gpointer data) {
   ((Grapher *)data)->reload_expr(true, false);
}

/**
 * Callback to reload expression and redraw graph with Riemann sum view
 *
 * @param widget The caller
 * @param data The Grapher object
 */
void load_expr_rs(GtkWidget *widget, gpointer data) {
   ((Grapher *)data)->reload_expr(false, true);
}

void Grapher::make_analysis() {
   GtkWidget *analysis_nb = gtk_notebook_new();
   gtk_grid_attach(GTK_GRID(grid), analysis_nb, 5, 2, 2, 8);
   
   // Making the trace menu
   GtkWidget *tr_grid = gtk_grid_new();
   gtk_grid_set_row_spacing(GTK_GRID(tr_grid), 15);
   gtk_grid_set_column_spacing(GTK_GRID(tr_grid), 10);
   gtk_widget_set_margin_top(tr_grid, 10);
   gtk_widget_set_margin_left(tr_grid, 10);
   gtk_widget_set_margin_right(tr_grid, 10);

   GtkWidget *tr_label = gtk_label_new("Eval");
   gtk_notebook_append_page(GTK_NOTEBOOK(analysis_nb),
                            tr_grid,
                            tr_label);

   GtkWidget *tr_xval_label = gtk_label_new("x =");
   gtk_grid_attach(GTK_GRID(tr_grid), tr_xval_label, 0, 0, 2, 1);

   tr_xval_entry = gtk_entry_new();
   gtk_grid_attach(GTK_GRID(tr_grid), tr_xval_entry, 2, 0, 2, 1);
   g_signal_connect(G_OBJECT(tr_xval_entry), "activate",
                    G_CALLBACK(load_expr_tr), this);

   GtkWidget *trace_button = gtk_button_new_with_label("Find");
   gtk_grid_attach(GTK_GRID(tr_grid), trace_button, 2, 1, 1, 1);
   g_signal_connect(G_OBJECT(trace_button), "clicked",
                    G_CALLBACK(load_expr_tr), this);
   
   GtkWidget *tr_res_label = gtk_label_new("y =");
   gtk_grid_attach(GTK_GRID(tr_grid), tr_res_label, 0, 2, 2, 1);

   tr_res_area = gtk_label_new("");
   gtk_grid_attach(GTK_GRID(tr_grid), tr_res_area, 2, 2, 2, 1);

   // Making the Riemann sum menu
   GtkWidget *rs_grid = gtk_grid_new();
   gtk_grid_set_row_spacing(GTK_GRID(rs_grid), 10);
   gtk_widget_set_margin_top(rs_grid, 10);
   gtk_widget_set_margin_left(rs_grid, 10);
   gtk_widget_set_margin_right(rs_grid, 10);
   
   GtkWidget *rs_label = gtk_label_new("RSum");
   gtk_notebook_append_page(GTK_NOTEBOOK(analysis_nb),
                            rs_grid,
                            rs_label);
   
   GtkWidget *rs_lower_label = gtk_label_new("xmin: ");
   gtk_grid_attach(GTK_GRID(rs_grid), rs_lower_label, 0, 0, 1, 1);
   
   rs_lower_entry = gtk_entry_new();
   gtk_grid_attach(GTK_GRID(rs_grid), rs_lower_entry, 1, 0, 1, 1);
   g_signal_connect(G_OBJECT(rs_lower_entry), "activate",
                    G_CALLBACK(load_expr_rs), this);

   GtkWidget *rs_upper_label = gtk_label_new("xmax: ");
   gtk_grid_attach(GTK_GRID(rs_grid), rs_upper_label, 0, 1, 1, 1);

   rs_upper_entry = gtk_entry_new();
   gtk_grid_attach(GTK_GRID(rs_grid), rs_upper_entry, 1, 1, 1, 1);
   g_signal_connect(G_OBJECT(rs_upper_entry), "activate",
                    G_CALLBACK(load_expr_rs), this);

   GtkWidget *rs_step_label = gtk_label_new("step: ");
   gtk_grid_attach(GTK_GRID(rs_grid), rs_step_label, 0, 2, 1, 1);

   rs_step_entry = gtk_entry_new();
   gtk_grid_attach(GTK_GRID(rs_grid), rs_step_entry, 1, 2, 1, 1);
   g_signal_connect(G_OBJECT(rs_step_entry), "activate",
                    G_CALLBACK(load_expr_rs), this);

   GtkWidget *sum_button = gtk_button_new_with_label("Sum");
   gtk_grid_attach(GTK_GRID(rs_grid), sum_button, 0, 3, 2, 1);
   g_signal_connect(G_OBJECT(sum_button), "clicked",
                    G_CALLBACK(load_expr_rs), this);

   GtkWidget *rs_res_label = gtk_label_new("Integral estimate:");
   gtk_grid_attach(GTK_GRID(rs_grid), rs_res_label, 0, 4, 2, 1);

   rs_res_area = gtk_label_new("");
   gtk_grid_attach(GTK_GRID(rs_grid), rs_res_area, 0, 5, 2, 1);

   // Making Monte Carlo menu
   GtkWidget *mc_grid = gtk_grid_new();
   gtk_grid_set_row_spacing(GTK_GRID(mc_grid), 10); 
   gtk_widget_set_margin_top(mc_grid, 10);
   gtk_widget_set_margin_left(mc_grid, 10);
   gtk_widget_set_margin_right(mc_grid, 10);

   GtkWidget *mc_label = gtk_label_new("MCarlo");
   gtk_notebook_append_page(GTK_NOTEBOOK(analysis_nb),
                            mc_grid,
                            mc_label);

   GtkWidget *mc_xmin_label = gtk_label_new("xmin: ");
   gtk_grid_attach(GTK_GRID(mc_grid), mc_xmin_label, 0, 0, 1, 1);

   mc_xmin_entry = gtk_entry_new();
   gtk_grid_attach(GTK_GRID(mc_grid), mc_xmin_entry, 1, 0, 1, 1);

   GtkWidget *mc_xmax_label = gtk_label_new("xmax: ");
   gtk_grid_attach(GTK_GRID(mc_grid), mc_xmax_label, 0, 1, 1, 1);

   mc_xmax_entry = gtk_entry_new();
   gtk_grid_attach(GTK_GRID(mc_grid), mc_xmax_entry, 1, 1, 1, 1);

   GtkWidget *mc_ymin_label = gtk_label_new("ymin: ");
   gtk_grid_attach(GTK_GRID(mc_grid), mc_ymin_label, 0, 2, 1, 1);

   mc_ymin_entry = gtk_entry_new();
   gtk_grid_attach(GTK_GRID(mc_grid), mc_ymin_entry, 1, 2, 1, 1);

   GtkWidget *mc_ymax_label = gtk_label_new("ymax: ");
   gtk_grid_attach(GTK_GRID(mc_grid), mc_ymax_label, 0, 3, 1, 1);

   mc_ymax_entry = gtk_entry_new();
   gtk_grid_attach(GTK_GRID(mc_grid), mc_ymax_entry, 1, 3, 1, 1);

   GtkWidget *approx_btn = gtk_button_new_with_label("Approximate");
   gtk_grid_attach(GTK_GRID(mc_grid), approx_btn, 0, 4, 2, 1);

   GtkWidget *approx_label = gtk_label_new("Integral estimate:");
   gtk_grid_attach(GTK_GRID(mc_grid), approx_label, 0, 5, 2, 1);

   mc_res_area = gtk_label_new("");
   gtk_grid_attach(GTK_GRID(mc_grid), mc_res_area, 0, 6, 2, 1);
}

void Grapher::make_all() {
   GtkWidget *window = gtk_application_window_new(app);
   gtk_window_set_title(GTK_WINDOW(window), "Grapher");
   gtk_container_set_border_width(GTK_CONTAINER(window), 10);

   grid = gtk_grid_new();
   gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
   gtk_container_add(GTK_CONTAINER(window), grid);

   make_grapher();
   make_settings();
   make_analysis();
   
   gtk_widget_show_all(window);
}

int Grapher::run(int argc, char **argv) {
   app = gtk_application_new("io.github.faulhat.riemann", G_APPLICATION_FLAGS_NONE);
   g_signal_connect(app, "activate", G_CALLBACK(activate), this);

   int status = g_application_run(G_APPLICATION(app), argc, argv);
   g_object_unref(app);

   return status;
}

