#ifndef BUFFER_H
#define BUFFER_H

#define INFO
#define DEBUG

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define BUFFER_MAX 32

typedef struct buffer
{
    pthread_mutex_t buffer_lock; /* lock protecting the buffer struct */
    pthread_cond_t fill;         /* buffer has at least one item in it */
    pthread_cond_t empty;        /* buffer has at least one empty slot */

    int fill_offset; /* used by producer */
    int use_offset;  /* used by consumer */
    int count;       /* item number now in buffer */
    int size;        /* max item number in buffer */
    int not_put;     /* remaining items to be put into buffer */
    int not_used;    /* items in buffer which haven't been used */
    int item_index;  /* 0 to item_num - 1 */

    int *space; /* circular buffer space limited by size */
} buffer_t;

bool buffer_empty(buffer_t *buf)
{
    return (buf->count == 0);
}

bool buffer_full(buffer_t *buf)
{
    return (buf->count == buf->size);
}

buffer_t *init_buffer(buffer_t *buf, int size, int item_num)
{
    if (size > BUFFER_MAX || size <= 0)
    {
#ifdef DEBUG
        printf("[ERROR]: Failed to initialize buffer. Invalid size.\n");
#endif
        return NULL;
    }

    buf->space = (int *)malloc(sizeof(int) * size);
    if (buf->space == NULL)
    {
#ifdef DEBUG
        printf("[ERROR]: Failed to initialize buffer.\n");
#endif
        return NULL;
    }

    pthread_mutex_init(&(buf->buffer_lock), NULL);
    pthread_cond_init(&(buf->fill), NULL);
    pthread_cond_init(&(buf->empty), NULL);

    buf->fill_offset = 0;
    buf->use_offset = 0;
    buf->count = 0;
    buf->size = size;
    buf->not_put = item_num;
    buf->not_used = item_num;
    buf->item_index = 0;

#ifdef INFO
    printf("[INFO]: Initialized buffer with %d slots\n", size);
#endif
    return buf;
}

bool destory_buffer(buffer_t *buf)
{
    if (buf && buf->space)
    {
        free(buf->space);
        return true;
    }

    return false;
}
#endif