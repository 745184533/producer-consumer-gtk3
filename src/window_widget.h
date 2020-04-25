#ifndef WINDOW_WIDGET_H
#define WINDOW_WIDGET_H
#include <gtk/gtk.h>
#include "buffer.h"
#include "async_get_put.h"

#define LABEL_GRID_NAME_BUFFER (32)
typedef struct windows_widgets
{
    GtkTextView *text_user_log;
    GtkTextBuffer *text_user_log_buffer;

    GtkEntry *entry_buffer_size;
    GtkEntry *entry_item_num;
    GtkEntry *entry_consumer_num;
    GtkEntry *entry_producer_num;

    GtkLabel *current_thread;
    GtkLabel *label_grid[BUFFER_MAX];
} window_widgets_t;

typedef struct button_args
{
    event_pool_t *event_pool;
    window_widgets_t *widgets;
} button_args_t;

button_args_t *
init_button_args(button_args_t *args, event_pool_t *pool, window_widgets_t *widgets)
{
    args->event_pool = pool;
    args->widgets = widgets;

    return args;
}

window_widgets_t *bind_window_widgets(GtkBuilder *builder, window_widgets_t *widgets)
{
    widgets->text_user_log = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "text_user_log"));
    widgets->text_user_log_buffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "text_user_log_buffer"));
    widgets->entry_buffer_size = GTK_ENTRY(gtk_builder_get_object(builder, "entry_buffer_size"));
    widgets->entry_item_num = GTK_ENTRY(gtk_builder_get_object(builder, "entry_item_num"));
    widgets->entry_consumer_num = GTK_ENTRY(gtk_builder_get_object(builder, "entry_consumer_num"));
    widgets->entry_producer_num = GTK_ENTRY(gtk_builder_get_object(builder, "entry_producer_num"));
    widgets->current_thread = GTK_LABEL(gtk_builder_get_object(builder, "label_current_thread"));

    for (int i = 0; i < BUFFER_MAX; i++)
    {
        char label_grid_name[LABEL_GRID_NAME_BUFFER];
        sprintf(label_grid_name, "label_grid_%d", i);
        widgets->label_grid[i] = GTK_LABEL(gtk_builder_get_object(builder, label_grid_name));
    }
    return widgets;
}

#endif