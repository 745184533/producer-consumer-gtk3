#ifndef PRODUCER_CONSUMER_H
#define PRODUCER_CONSUMER_H

#define INFO
#define DEBUG

#define ASYNC_TIMEOUT_STEP (1000) /* ms */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include <gtk/gtk.h>
#include "window_widget.h"
#include "buffer.h"
#include "thread_id.h"
#include "async_get_put.h"

/* To calculate thread numbers in runtime, we have to pack and parse these args */
typedef struct thread_args
{
    buffer_t *buffer;          /* buffer that thread works on */
    window_widgets_t *widgets; /* GTK widgets*/
    event_pool_t *pool;        /* asynchornous UI */
} thread_args_t;

thread_args_t *
init_thread_args(thread_args_t *args, buffer_t *buf,
                 window_widgets_t *widgets, event_pool_t *pool)
{
    args->buffer = buf;
    args->widgets = widgets;
    args->pool = pool;

    return args;
}

void put(buffer_t *buf, window_widgets_t *widgets, event_pool_t *pool)
{
    /* put value */
    int fill = buf->fill_offset;
    int size = buf->size;
    int value = buf->item_index;
    buf->space[fill] = value;
    buf->fill_offset = (fill + 1) % size;

    /* update buffer info */
    buf->count++;
    buf->item_index++;
    buf->not_put--;
#ifdef DEBUG
    printf("[DEBUG]: Producer updated buffer info: count = %d item_index = %d not_put = %d\n",
           buf->count, buf->item_index, buf->not_put);
#endif

    GtkTextBuffer *user_log_buf = widgets->text_user_log_buffer;
    GtkLabel **buffer_monitor_labels = widgets->label_grid;
    GtkLabel *current_thread = widgets->current_thread;
    /* Asynchornous UI */
    pthread_mutex_lock(&(pool->lock));
#ifdef DEBUG
    printf("[DEBUG]: Producer got event pool lock.\n");
#endif

    async_args_t *async_args = &(pool->args[pool->offset]);
    pool->offset++;
    int timeout = pool->slowmotion_clock;
    pool->slowmotion_clock += ASYNC_TIMEOUT_STEP;
    pthread_t tid = pthread_self();
    init_async_args(async_args, value, fill, tid, user_log_buf, buffer_monitor_labels, current_thread);
    g_timeout_add(timeout, async_put, async_args);

#ifdef DEBUG
    printf("[DEBUG]: Producer is releasing event pool lock.\n");
#endif
    pthread_mutex_unlock(&(pool->lock));

#ifdef INFO
    printf("[INFO]: Put %d into buffer\n", value);
#endif
}

int get(buffer_t *buf, window_widgets_t *widgets, event_pool_t *pool)
{
    /* get from buffer */
    int use = buf->use_offset;
    int size = buf->size;
    int value = buf->space[use];
    buf->use_offset = (use + 1) % size;

    /* update buffer info */
    buf->count--;
    buf->not_used--;
#ifdef DEBUG
    printf("[DEBUG]: PConsumer updated buffer info: count = %d item_index = %d not_used = %d\n",
           buf->count, buf->item_index, buf->not_used);
#endif

    GtkTextBuffer *user_log_buf = widgets->text_user_log_buffer;
    GtkLabel **buffer_monitor_labels = widgets->label_grid;
    GtkLabel *current_thread = widgets->current_thread;

    /* Asynchornous UI */
    pthread_mutex_lock(&(pool->lock));
#ifdef DEBUG
    printf("[DEBUG]: Consumer got event pool lock.\n");
#endif

    async_args_t *async_args = &(pool->args[pool->offset]);
    pool->offset++;
    int timeout = pool->slowmotion_clock;
    pool->slowmotion_clock += ASYNC_TIMEOUT_STEP;
    pthread_t tid = pthread_self();
    init_async_args(async_args, value, use, tid, user_log_buf, buffer_monitor_labels, current_thread);
    g_timeout_add(timeout, async_get, async_args);

#ifdef DEBUG
    printf("[DEBUG]: Consumer is releasing event pool lock.\n");
#endif
    pthread_mutex_unlock(&(pool->lock));

#ifdef INFO
    printf("[INFO]: Get %d from buffer\n", value);
#endif

    return value;
}

void *producer(void *thread_args)
{
    thread_args_t *args = (thread_args_t *)thread_args;
    buffer_t *buf = args->buffer;
    window_widgets_t *widgets = args->widgets;
    event_pool_t *pool = args->pool;

    while (true)
    {
        pthread_mutex_lock(&(buf->buffer_lock));
#ifdef DEBUG
        printf("[DEBUG]: Producer got buffer lock.\n");
#endif
        while (buffer_full(buf) && buf->not_used > 0)
        {
#ifdef DEBUG
            printf("[DEBUG]: Producer is waiting condvar.\n");
#endif
            pthread_cond_wait(&(buf->empty), &(buf->buffer_lock));
        }

        /* Job done */
        if (buf->not_put == 0)
        {
            pthread_cond_signal(&(buf->fill));
            pthread_mutex_unlock(&(buf->buffer_lock));
#ifdef DEBUG
            printf("[DEBUG]: One producer got job done.\n");
#endif
            return NULL;
        }

        put(buf, widgets, pool);
        pthread_cond_signal(&(buf->fill));
#ifdef DEBUG
        printf("[DEBUG]: Producer is releasing buffer lock.\n");
#endif
        pthread_mutex_unlock(&(buf->buffer_lock));
    }
    return NULL;
}

void *consumer(void *thread_args)
{
    thread_args_t *args = (thread_args_t *)thread_args;
    buffer_t *buf = args->buffer;
    window_widgets_t *widgets = args->widgets;
    event_pool_t *pool = args->pool;

    while (true)
    {
        pthread_mutex_lock(&(buf->buffer_lock));
#ifdef DEBUG
        printf("[DEBUG]: Consumer got buffer lock.\n");
#endif
        while (buffer_empty(buf) && buf->not_used > 0)
        {
#ifdef DEBUG
            printf("[DEBUG]: Consumer is waiting condvar.\n");
#endif
            pthread_cond_wait(&(buf->fill), &(buf->buffer_lock));
        }

        /* Job done */
        if (buf->not_used == 0)
        {
            pthread_cond_signal(&(buf->empty));
            pthread_mutex_unlock(&(buf->buffer_lock));
#ifdef DEBUG
            printf("[DEBUG]: One consumer got job done.\n");
#endif
            return NULL;
        }

        get(buf, widgets, pool);
        pthread_cond_signal(&(buf->empty));
#ifdef DEBUG
        printf("[DEBUG]: Consumer is releasing buffer lock.\n");
#endif
        pthread_mutex_unlock(&(buf->buffer_lock));
    }
    return NULL;
}

void create_consumer(thread_args_t *args, thread_ids_t *csm)
{
    int consumer_num = csm->count;
    for (int i = 0; i < consumer_num; i++)
    {
        pthread_create(&(csm->thread_id[i]), NULL, consumer, args);
    }
}

void join_consumer(thread_ids_t *csm)
{
    int consumer_num = csm->count;
    for (int i = 0; i < consumer_num; i++)
    {
        pthread_join(csm->thread_id[i], NULL);
    }
}

void create_producer(thread_args_t *args, thread_ids_t *prod)
{
    int producer_num = prod->count;
    for (int i = 0; i < producer_num; i++)
    {
        pthread_create(&(prod->thread_id[i]), NULL, producer, args);
    }
}

void join_producer(thread_ids_t *prod)
{
    int producer_num = prod->count;
    for (int i = 0; i < producer_num; i++)
    {
        pthread_join(prod->thread_id[i], NULL);
    }
}

#endif