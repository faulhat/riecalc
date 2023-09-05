#include "grapher.hpp"

#include <cmath>

gboolean draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
   static const GdkRGBA RED = {1.0, 0.0, 0.0, 1.0};

   Grapher *parent = (Grapher *)data;

   GtkStyleContext *ctx = gtk_widget_get_style_context(widget);
   guint width = gtk_widget_get_allocated_width(widget),
         height = gtk_widget_get_allocated_height(widget);

   gtk_render_background(ctx, cr, 0, 0, width, height);

   const Func &fn = parent->getFn();
   if (fn != nullptr) {
      gdk_cairo_set_source_rgba(cr, &RED);

      for (double i = 0.0; i < width; i += 10.0) {
         double val = fn(i);
         cairo_arc(cr, i, val, 5, 0, 2 * G_PI);
         cairo_fill(cr);
      }
   }

   return FALSE;
}

void activate(GtkApplication *app, gpointer data) {
   ((Grapher *)data)->activate_graph();
}

Grapher::Grapher() {
   app = gtk_application_new("io.github.faulhat.riemann", G_APPLICATION_FLAGS_NONE);
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

void Grapher::activate_graph() {
   window = gtk_application_window_new(app);
   gtk_window_set_title(GTK_WINDOW(window), "Grapher");
   gtk_window_set_default_size(GTK_WINDOW(window), 1000, 750);

   graphing_area = gtk_drawing_area_new();
   gtk_widget_set_size_request(graphing_area, 100, 100);
   g_signal_connect(G_OBJECT(graphing_area), "draw",
                    G_CALLBACK(draw), this);

   gtk_container_add(GTK_CONTAINER(window), graphing_area);
   gtk_widget_show_all(window);
}

int Grapher::run(int argc, char **argv) {
   fn = &sqrt;
   g_signal_connect(app, "activate", G_CALLBACK(activate), this);

   int status = g_application_run(G_APPLICATION(app), argc, argv);
   g_object_unref(app);

   return status;
}

const Func &Grapher::getFn() const {
   return fn;
}

