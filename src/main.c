#include <gtk/gtk.h>
#include "window_logic.h"

int main(int argc, char *argv[])
{
    /* Initialize data structures */
    GtkBuilder *builder;
    GtkWidget *window;
    window_widgets_t *widgets = g_slice_new(window_widgets_t);
    event_pool_t *event_pool = g_slice_new(event_pool_t);
    button_args_t *button_args = g_slice_new(button_args_t);
    init_event_pool(event_pool, MAX_EVENT);
    init_button_args(button_args, event_pool, widgets);

    gtk_init(&argc, &argv);

    /* GTK builder */
    builder = gtk_builder_new_from_file("view/window_main.xml");
    bind_window_widgets(builder, widgets);
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    gtk_builder_connect_signals(builder, button_args);
    g_object_unref(builder);

    /* Main window */
    gtk_widget_show(window);
    gtk_main();

    /* Free memory */
    g_slice_free(window_widgets_t, widgets);
    destory_event_pool(event_pool);
    g_slice_free(event_pool_t, event_pool);
    g_slice_free(button_args_t, button_args);

    return 0;
}

void on_window_main_destroy()
{
    gtk_main_quit();
}
