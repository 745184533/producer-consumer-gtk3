#ifndef ASYNC_GET_PUT_H
#define ASYNC_GET_PUT_H

#include <gtk/gtk.h>
#include <stdbool.h>
#include <pthread.h>

#include "window_widget.h"

#define MAX_ITEM (512)
#define MAX_EVENT (MAX_ITEM * (2))

#define PRINT_TEXT_BUFFER (64)
#define LABEL_TEXT_BUFFER (64)

#define ASYNC_TIMEOUT_FIRST (1000) /* ms */
typedef struct async_args
{
    int value;
    int grid_offset;
    pthread_t tid;
    GtkTextBuffer *buffer;
    GtkLabel *current_thread;
    GtkLabel **label_grid;
} async_args_t;

typedef struct event_pool
{
    pthread_mutex_t lock;
    int offset;
    int slowmotion_clock;
    async_args_t *args;
} event_pool_t;

event_pool_t *init_event_pool(event_pool_t *pool, int size)
{
    if (pool == NULL)
    {
#ifdef DEBUG
        printf("[ERROR]: Invalid pointer, failed to initialize event pool.\n");
#endif
        return NULL;
    }

    if (size <= 0 || size > MAX_EVENT)
    {
#ifdef DEBUG
        printf("[ERROR]: Invalid size, failed to initialize event pool.\n");
#endif
        return pool;
    }

    pool->args = (async_args_t *)malloc(sizeof(async_args_t) * size);
    if (pool->args == NULL)
    {
#ifdef DEBUG
        printf("[ERROR]: Failed to allocate memory space for event pool.\n");
#endif
    }

    pool->offset = 0;
    pool->slowmotion_clock = ASYNC_TIMEOUT_FIRST;
    pthread_mutex_init(&(pool->lock), NULL);
    return pool;
}

bool destory_event_pool(event_pool_t *pool)
{
    if (pool && pool->args)
    {
        free(pool->args);
        return true;
    }

    return false;
}

async_args_t *
init_sync_args(async_args_t *args, int value, int grid_offset, pthread_t tid,
               GtkTextBuffer *buffer, GtkLabel **label_grid, GtkLabel *current_thread)
{
    args->value = value;
    args->grid_offset = grid_offset;
    args->tid = tid;
    args->buffer = buffer;
    args->label_grid = label_grid;
    args->current_thread = current_thread;

    return args;
}

int async_get(void *async_args)
{
#ifdef DEBUG
    printf("[DEBUG]: async_get is called.\n");
#endif

    async_args_t *args = (async_args_t *)async_args;
    GtkTextBuffer *buffer = args->buffer;
    GtkLabel **label_grid = args->label_grid;
    GtkLabel *current_thread = args->current_thread;
    int value = args->value;
    int grid_offset = args->grid_offset;
    pthread_t tid = args->tid;

    /* print to GTK text view */
    char text_to_print[PRINT_TEXT_BUFFER];
    sprintf(text_to_print, "[INFO]: Get %d from buffer\n", value);
    int len = strlen(text_to_print);
    gtk_text_buffer_insert_at_cursor(buffer, text_to_print, len);

    /* set GTK label */
    char label_text[LABEL_TEXT_BUFFER] = "Used";
    gtk_label_set_text(label_grid[grid_offset], label_text);
    char current_thread_text[LABEL_TEXT_BUFFER];
    sprintf(current_thread_text, "Consumer TID %ld", tid);
    gtk_label_set_text(current_thread, current_thread_text);
    return 0;
}

int async_put(void *async_args)
{
#ifdef DEBUG
    printf("[DEBUG]: async_put is called.\n");
#endif

    async_args_t *args = (async_args_t *)async_args;
    GtkTextBuffer *buffer = args->buffer;
    GtkLabel **label_grid = args->label_grid;
    GtkLabel *current_thread = args->current_thread;
    int value = args->value;
    int grid_offset = args->grid_offset;
    pthread_t tid = args->tid;

    /* print to GTK text view */
    char text_to_print[PRINT_TEXT_BUFFER];
    sprintf(text_to_print, "[INFO]: Put %d into buffer\n", value);
    int len = strlen(text_to_print);
    gtk_text_buffer_insert_at_cursor(buffer, text_to_print, len);

    /* set GTK label */
    char label_text[LABEL_TEXT_BUFFER] = "Item";
    gtk_label_set_text(label_grid[grid_offset], label_text);
    char current_thread_text[LABEL_TEXT_BUFFER];
    sprintf(current_thread_text, "Producer TID %ld", tid);
    gtk_label_set_text(current_thread, current_thread_text);
    return 0;
}

#endif