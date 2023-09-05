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
   cairo_move_to(cr, 0, height / 2.0);
   cairo_line_to(cr, width, height / 2.0);
   cairo_stroke(cr);

   cairo_move_to(cr, width / 2.0, 0);
   cairo_line_to(cr, width / 2.0, height);
   cairo_stroke(cr);

   if (fn != nullptr) {
      gdk_cairo_set_source_rgba(cr, &RED);

      for (double x = 0.0; x < width; x += 1.0) {
         double y = height / 2.0 - fn(x - width / 2.0);
         cairo_line_to(cr, x, y);
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
   gtk_widget_queue_draw(graphing_area);
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
   const char *expr_str = gtk_entry_get_text((GtkEntry *)expr_entry);
   apply_fn_str(expr_str);
}

void Grapher::activate_graph() {
   window = gtk_application_window_new(app);
   gtk_window_set_title(GTK_WINDOW(window), "Grapher");
   gtk_container_set_border_width(GTK_CONTAINER(window), 10);

   grid = gtk_grid_new();
   gtk_container_add(GTK_CONTAINER(window), grid);

   expr_entry = gtk_entry_new();
   gtk_entry_set_max_length(GTK_ENTRY(expr_entry), 0);
   gtk_grid_attach(GTK_GRID(grid), expr_entry, 0, 0, 4, 1);
   g_signal_connect(G_OBJECT(expr_entry), "activate",
                    G_CALLBACK(load_expr), this);
   
   go_button = gtk_button_new_with_label("Go");
   gtk_grid_attach(GTK_GRID(grid), go_button, 4, 0, 1, 1);
   g_signal_connect(G_OBJECT(go_button), "clicked",
                    G_CALLBACK(load_expr), this);

   graphing_area = gtk_drawing_area_new();
   gtk_widget_set_size_request(graphing_area, 1000, 500);
   gtk_grid_attach(GTK_GRID(grid), graphing_area, 0, 1, 5, 1);
   g_signal_connect(G_OBJECT(graphing_area), "draw",
                    G_CALLBACK(draw), this);

   gtk_widget_show_all(window);
}

int Grapher::run(int argc, char **argv) {
   app = gtk_application_new("io.github.faulhat.riemann", G_APPLICATION_FLAGS_NONE);
   g_signal_connect(app, "activate", G_CALLBACK(activate), this);

   int status = g_application_run(G_APPLICATION(app), argc, argv);
   g_object_unref(app);

   return status;
}

