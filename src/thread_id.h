#ifndef THREAD_ID_H
#define THREAD_ID_H

#define INFO
#define DEBUG

#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define THREAD_MAX 8
/* store thread IDs */
typedef struct thread_ids
{
    int count;
    pthread_t *thread_id;
} thread_ids_t;

thread_ids_t *init_thread_ids(thread_ids_t *tids, int num)
{
    if (num > THREAD_MAX || num <= 0)
    {
#ifdef DEBUG
        printf("[ERROR]: Failed to initialize thread IDs. Invalid number.\n");
#endif
        return NULL;
    }
    
    tids->thread_id = (pthread_t *)malloc(sizeof(pthread_t) * num);
    if (tids->thread_id == NULL)
    {
#ifdef DEBUG
        printf("[ERROR]: Failed to initialize thread IDs.\n");
#endif
        return NULL;
    }

    tids->count = num;
    return tids;
}

bool destory_thread_ids(thread_ids_t *tids)
{
    if (tids && tids->thread_id)
    {
        free(tids->thread_id);
        return true;
    }
    return false;
}

#endif