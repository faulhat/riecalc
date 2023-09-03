#include <gtk/gtk.h>

gboolean draw_callback(GtkWidget *widget, cairo_t *cr, gpointer data) {
   guint width, height;
   GdkRGBA color;
   GtkStyleContext *context;

   context = gtk_widget_get_style_context(widget);

   width = gtk_widget_get_allocated_width(widget);
   height = gtk_widget_get_allocated_height(widget);

   gtk_render_background(context, cr, 0, 0, width, height);

   cairo_arc(cr,
             width / 2.0, height / 2.0,
             MIN(width, height) / 2.0,
             0, 2 * G_PI);

   gtk_style_context_get_color(context,
                               gtk_style_context_get_state(context),
                               &color);
   gdk_cairo_set_source_rgba(cr, &color);

   cairo_fill(cr);

   return FALSE;
}

static void activate(GtkApplication* app, gpointer user_data) {
  	GtkWidget *window;

  	window = gtk_application_window_new (app);
  	gtk_window_set_title (GTK_WINDOW (window), "Window");
  	gtk_window_set_default_size (GTK_WINDOW (window), 1000, 750);

	GtkWidget *drawing_area = gtk_drawing_area_new();
	gtk_widget_set_size_request(drawing_area, 100, 100);
	g_signal_connect(G_OBJECT(drawing_area), "draw",
						  G_CALLBACK(draw_callback), NULL);
	
	gtk_container_add(GTK_CONTAINER(window), drawing_area);
  	gtk_widget_show_all(window);
}

int graph_main(int argc, char **argv) {
  GtkApplication *app;
  int status;

   app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
   g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
   status = g_application_run(G_APPLICATION(app), argc, argv);
   g_object_unref(app);

   return status;
}

