#ifndef WINDOW_LOGIC_H
#define WINDOW_LOGIC_H

#include <gtk/gtk.h>
#include <string.h>
#include <locale.h>

#include "buffer.h"
#include "producer_consumer.h"
#include "thread_id.h"
#include "user_input.h"
#include "window_widget.h"

#define BUTTON_TIMEOUT (100) /* ms */
typedef struct workload_args
{
    window_widgets_t *widgets;
    metadata_t *mdata;
    event_pool_t *pool;
} workload_args_t;

workload_args_t *
init_workload_args(workload_args_t *args, window_widgets_t *widgets,
                   metadata_t *mdata, event_pool_t *pool)
{
    args->widgets = widgets;
    args->mdata = mdata;
    args->pool = pool;

    return args;
}

int workload(void *workload_args)
{
    workload_args_t *args = (workload_args_t *)workload_args;
    window_widgets_t *widgets = args->widgets;
    metadata_t *mdata = args->mdata;
    event_pool_t *pool = args->pool;

    if (widgets == NULL || mdata == NULL)
    {
#ifdef DEBUG
        printf("[ERROR]: Invalid pointer for workload. Fatal error, exit.\n");
#endif
        return 0;
    }

    if (!valid_metadata(mdata))
    {
#ifdef DEBUG
        printf("[ERROR]: Invalid metadata for workload.\n");
#endif
        return 0;
    }

    /* initialize */
    buffer_t buffer;
    init_buffer(&buffer, mdata->buffer_size, mdata->item_num);
    thread_ids_t consumers;
    init_thread_ids(&consumers, mdata->consumer_num);
    thread_ids_t producers;
    init_thread_ids(&producers, mdata->producer_num);
    thread_args_t consumer_args;
    init_thread_args(&consumer_args, &buffer, widgets, pool);
    thread_args_t producer_args;
    init_thread_args(&producer_args, &buffer, widgets, pool);

    /* create threads then wait them*/
    create_producer(&producer_args, &producers);
    create_consumer(&consumer_args, &consumers);
    join_consumer(&consumers);
    join_producer(&producers);

    /* free memory */
    destory_buffer(&buffer);
    destory_thread_ids(&consumers);
    destory_thread_ids(&producers);

#ifdef INFO
    printf("[INFO]: Done!\n");
#endif

    return 0;
}

void on_button_start_clicked(GtkButton *button, button_args_t *args)
{
#ifdef DEBUG
    printf("[DEBUG]: Start button clicked\n");
#endif

    window_widgets_t *widgets = args->widgets;
    event_pool_t *pool = args->event_pool;
    static metadata_t mdata;
    input_metadata_gtk(&mdata, widgets);
    static workload_args_t workload_args;
    init_workload_args(&workload_args, widgets, &mdata, pool);

#ifdef DEBUG
    printf("[DEBUG]: Start button timeout callback added.\n");
#endif
    /* Asynchornous */
    g_timeout_add(BUTTON_TIMEOUT, workload, &workload_args);
}

void on_button_clear_clicked(GtkButton *button, button_args_t *args)
{
#ifdef DEBUG
    printf("[DEBUG]: Clear button clicked\n");
#endif
    window_widgets_t *widgets = args->widgets;
    
    /* set GTK text view */
    char clear_text[] = "User Logs: \n";
    int len = strlen(clear_text);
    gtk_text_buffer_set_text(widgets->text_user_log_buffer, clear_text, len);

    /* set GTK label */
    for (int i = 0; i < BUFFER_MAX; i++)
    {
        char label_text[LABEL_TEXT_BUFFER] = "Null";
        gtk_label_set_text(widgets->label_grid[i], label_text);
    }
}

#endif