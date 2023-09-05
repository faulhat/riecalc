#include "grapher.hpp"

#include <cmath>

gboolean draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
   return ((Grapher *)data)->draw_graph(cr);
}

gboolean Grapher::draw_graph(cairo_t *cr) {
   static const GdkRGBA RED = {1.0, 0.0, 0.0, 1.0},
                        FOG = {0.3, 0.3, 0.3, 1.0},
                        SKY = {135.0/255.0,
                               206.0/255.0,
                               235.0/255.0,
                               1.0};
   GtkStyleContext *ctx = gtk_widget_get_style_context(graphing_area);
   guint width = gtk_widget_get_allocated_width(graphing_area),
         height = gtk_widget_get_allocated_height(graphing_area);

   gtk_render_background(ctx, cr, 0, 0, width, height);

   gdk_cairo_set_source_rgba(cr, &SKY);
   cairo_rectangle(cr, 0, 0, width, height);
   cairo_fill(cr);

   gdk_cairo_set_source_rgba(cr, &FOG);
   double y_zero_line = height * (1 - (-ymin / (ymax - ymin)));
   if (y_zero_line > 0 && y_zero_line < height) {
      cairo_move_to(cr, 0, y_zero_line);
      cairo_line_to(cr, width, y_zero_line);
      cairo_stroke(cr);
   }

   double x_zero_line = width * -xmin / (xmax - xmin);
   if (x_zero_line > 0 && x_zero_line < width) {
      cairo_move_to(cr, x_zero_line, 0);
      cairo_line_to(cr, x_zero_line, height);
      cairo_stroke(cr);
   }

   if (fn != nullptr) {
      gdk_cairo_set_source_rgba(cr, &RED);

      for (guint i = 0; i < width; i++) {
         double x = i * (xmax - xmin) / width + xmin;
         double y = fn(x);
         int j = (int)(height * (1 - (y - ymin) / (ymax - ymin)));
         cairo_line_to(cr, i, j);
      }

      cairo_stroke(cr);
   }

   return FALSE;
}

void activate(GtkApplication *app, gpointer data) {
   ((Grapher *)data)->activate_graph();
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
   
   // TODO: fix this idk
   if (err != nullptr) return;

   apply_expr(expr);

   if (funcname != nullptr) {
      free(funcname);
   }

   if (varname != nullptr) {
      free(varname);
   }
   yylex_destroy();
}

void load_expr(GtkWidget *widget, gpointer data) {
   ((Grapher *)data)->reload_expr();
}

void Grapher::reload_expr() {
   const char *fmt = "%lf%c";
   char extra;
   int tokens = sscanf(gtk_entry_get_text(GTK_ENTRY(xmin_entry)),
                                          fmt, &xmin, &extra);
   if (tokens != 1) {
      gtk_label_set_text(GTK_LABEL(err_area),
                         "Error: Could not parse xmin bound.");
      return;
   }

   tokens = sscanf(gtk_entry_get_text(GTK_ENTRY(xmax_entry)),
                                      fmt, &xmax, &extra);
   if (tokens != 1) {
      gtk_label_set_text(GTK_LABEL(err_area),
                         "Error: Could not parse xmax bound.");
      return;
   }

   tokens = sscanf(gtk_entry_get_text(GTK_ENTRY(ymin_entry)),
                                      fmt, &ymin, &extra);
   if (tokens != 1) {
      gtk_label_set_text(GTK_LABEL(err_area),
                         "Error: Could not parse ymin bound.");
      return;
   }

   tokens = sscanf(gtk_entry_get_text(GTK_ENTRY(ymax_entry)),
                                      fmt, &ymax, &extra);
   if (tokens != 1) {
      gtk_label_set_text(GTK_LABEL(err_area),
                         "Error: Could not parse ymax bound.");
      return;
   }

   try {
      const char *expr_str = gtk_entry_get_text(GTK_ENTRY(expr_entry));
      apply_fn_str(expr_str);
   } catch (NameResFail *e) {
      gtk_label_set_text(GTK_LABEL(err_area), e->what());
      delete e;
      return;
   } catch (ParseError *e) {
      gtk_label_set_text(GTK_LABEL(err_area), e->what());
      delete e;
      return;
   }

   gtk_label_set_text(GTK_LABEL(err_area), "");
   gtk_widget_queue_draw(graphing_area);
}

void Grapher::activate_graph() {
   window = gtk_application_window_new(app);
   gtk_window_set_title(GTK_WINDOW(window), "Grapher");
   gtk_container_set_border_width(GTK_CONTAINER(window), 10);

   grid = gtk_grid_new();
   gtk_container_add(GTK_CONTAINER(window), grid);

   GtkWidget *expr_label = gtk_label_new("Enter an expression in terms of x:");
   gtk_grid_attach(GTK_GRID(grid), expr_label, 0, 0, 1, 1);
   
   expr_entry = gtk_entry_new();
   gtk_entry_set_max_length(GTK_ENTRY(expr_entry), 0);
   gtk_grid_attach(GTK_GRID(grid), expr_entry, 0, 1, 4, 1);
   g_signal_connect(G_OBJECT(expr_entry), "activate",
                    G_CALLBACK(load_expr), this);
   
   go_button = gtk_button_new_with_label("Go");
   gtk_grid_attach(GTK_GRID(grid), go_button, 4, 1, 1, 1);
   g_signal_connect(G_OBJECT(go_button), "clicked",
                    G_CALLBACK(load_expr), this);

   graphing_area = gtk_drawing_area_new();
   gtk_widget_set_size_request(graphing_area, 1000, 500);
   gtk_grid_attach(GTK_GRID(grid), graphing_area, 0, 2, 5, 5);
   g_signal_connect(G_OBJECT(graphing_area), "draw",
                    G_CALLBACK(draw), this);

   GtkWidget *xmin_label = gtk_label_new("xMin");
   gtk_grid_attach(GTK_GRID(grid), xmin_label, 1, 7, 1, 1);

   GtkWidget *xmax_label = gtk_label_new("xMax");
   gtk_grid_attach(GTK_GRID(grid), xmax_label, 2, 7, 1, 1);

   GtkWidget *ymin_label = gtk_label_new("yMin");
   gtk_grid_attach(GTK_GRID(grid), ymin_label, 3, 7, 1, 1);

   GtkWidget *ymax_label = gtk_label_new("yMax");
   gtk_grid_attach(GTK_GRID(grid), ymax_label, 4, 7, 1, 1);

   GtkWidget *dim_label = gtk_label_new("Window:");
   gtk_grid_attach(GTK_GRID(grid), dim_label, 0, 8, 1, 1);

   xmin_entry = gtk_entry_new();
   gtk_entry_set_max_length(GTK_ENTRY(xmin_entry), 0);
   gtk_entry_set_text(GTK_ENTRY(xmin_entry), "-10.0");
   gtk_grid_attach(GTK_GRID(grid), xmin_entry, 1, 8, 1, 1);
   g_signal_connect(G_OBJECT(xmin_entry), "activate",
                    G_CALLBACK(load_expr), this);
   xmin = -10.0;

   xmax_entry = gtk_entry_new();
   gtk_entry_set_max_length(GTK_ENTRY(xmax_entry), 0);
   gtk_entry_set_text(GTK_ENTRY(xmax_entry), "10.0");
   gtk_grid_attach(GTK_GRID(grid), xmax_entry, 2, 8, 1, 1);
   g_signal_connect(G_OBJECT(xmax_entry), "activate",
                    G_CALLBACK(load_expr), this);
   xmax = 10.0;

   ymin_entry = gtk_entry_new();
   gtk_entry_set_max_length(GTK_ENTRY(ymin_entry), 0);
   gtk_entry_set_text(GTK_ENTRY(ymin_entry), "-10.0");
   gtk_grid_attach(GTK_GRID(grid), ymin_entry, 3, 8, 1, 1);
   g_signal_connect(G_OBJECT(ymin_entry), "activate",
                    G_CALLBACK(load_expr), this);
   ymin = -10.0;

   ymax_entry = gtk_entry_new();
   gtk_entry_set_max_length(GTK_ENTRY(ymax_entry), 0);
   gtk_entry_set_text(GTK_ENTRY(ymax_entry), "10.0");
   gtk_grid_attach(GTK_GRID(grid), ymax_entry, 4, 8, 1, 1);
   g_signal_connect(G_OBJECT(ymax_entry), "activate",
                    G_CALLBACK(load_expr), this);
   ymax = 10.0;

   err_area = gtk_label_new("");
   gtk_grid_attach(GTK_GRID(grid), err_area, 0, 9, 1, 1);

   gtk_widget_show_all(window);
}

int Grapher::run(int argc, char **argv) {
   app = gtk_application_new("io.github.faulhat.riemann", G_APPLICATION_FLAGS_NONE);
   g_signal_connect(app, "activate", G_CALLBACK(activate), this);

   int status = g_application_run(G_APPLICATION(app), argc, argv);
   g_object_unref(app);

   return status;
}

