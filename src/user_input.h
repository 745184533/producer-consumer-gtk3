#ifndef USER_INPUT_H
#define USER_INPUT_H

#include <stdio.h>

typedef struct metadata
{
    int producer_num;
    int consumer_num;
    int item_num;
    int buffer_size;
    bool valid;
} metadata_t;

bool valid_metadata(metadata_t *mdata)
{
    return (mdata->valid);
}
void print_metadata(metadata_t *mdata)
{
    int csm_num = mdata->consumer_num;
    int prod_num = mdata->producer_num;
    int item_num = mdata->item_num;
    int buffer_size = mdata->buffer_size;

    printf("[DEBUG]: consumer_num = %d\n", csm_num);
    printf("[DEBUG]: producer_num = %d\n", prod_num);
    printf("[DEBUG]: item_num = %d\n", item_num);
    printf("[DEBUG]: buffer_size = %d\n", buffer_size);
}

metadata_t *init_metadata(metadata_t *mdata, int buffer_size, int item_num, int producer_num, int consumer_num)
{
    if (mdata == NULL)
    {
#ifdef DEBUG
        printf("[ERROR]: Invalid pointer in init_metadata.\n");
#endif
        return NULL;
    }

    if (producer_num <= 0 || producer_num > THREAD_MAX || consumer_num <= 0 || consumer_num > THREAD_MAX)
    {
#ifdef DEBUG
        printf("[ERROR]: Invalid thread number.\n");
#endif
        mdata->valid = false;
        return mdata; /* to free mdata */
    }

    if (buffer_size <= 0 || buffer_size > BUFFER_MAX ||  item_num <= 0 || item_num > MAX_ITEM)
    {
#ifdef DEBUG
        printf("[ERROR]: Invalid item number and buffer size.\n");
#endif
        mdata->valid = false;
        return mdata; /* to free mdata */
    }

    mdata->buffer_size = buffer_size;
    mdata->item_num = item_num;
    mdata->producer_num = producer_num;
    mdata->consumer_num = consumer_num;

    mdata->valid = true;
#ifdef DEBUG
    print_metadata(mdata);
#endif
    return mdata;
}

int get_int_from_entry(GtkEntry *entry)
{
    return (atoi(gtk_entry_get_text(entry)));
}

metadata_t *input_metadata_gtk(metadata_t *mdata, window_widgets_t *widgets)
{
    int buffer_size = get_int_from_entry(widgets->entry_buffer_size);
    int item_num = get_int_from_entry(widgets->entry_item_num);
    int producer_num = get_int_from_entry(widgets->entry_producer_num);
    int consumer_num = get_int_from_entry(widgets->entry_consumer_num);
    init_metadata(mdata, buffer_size, item_num, producer_num, consumer_num);

    return mdata;
}

metadata_t *input_metadata_cli(metadata_t *mdata)
{
    int producer_num, consumer_num, item_num, buffer_size;
    printf("producer number: ");
    scanf("%d", &producer_num);
    getchar();
    printf("consumer number: ");
    scanf("%d", &consumer_num);
    getchar();
    printf("item number: ");
    scanf("%d", &item_num);
    getchar();
    printf("buffer size: ");
    scanf("%d", &buffer_size);
    getchar();

    init_metadata(mdata, buffer_size, item_num, producer_num, consumer_num);

    return mdata;
}

#endif