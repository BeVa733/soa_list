#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "spisok.h"

int main (int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("Incorrect n arguments (must be 2)\n");
        return 1;
    }

    spisok list = {};
    list_ctor (&list, 10);

    insert_after(&list, 0, 10);
    insert_before(&list, 1, 22);
    list_graph(&list, argv[1]);
    insert_after(&list, 2, 30);
    list_delete(&list, 2);
    insert_after(&list, 2, 25);
    list_graph(&list, argv[1]);

    list_dump(&list, 0);

    return 0;
}

void list_ctor(spisok* list, int size)
{
    list->data = (int*)calloc(size, sizeof(list->data[0]));
    list->next = (int*)calloc(size, sizeof(list->next[0]));
    list->prev = (int*)calloc(size, sizeof(list->prev[0]));
    list->size = size;

    for(int i = 0; i < size; i++)
    {
        list->data[i] = POIZON;
        list->next[i] = i + 1;
        list->prev[i] = POIZON;
    }
    list->next[size - 1] = POIZON;
    list->next[0] = 0;
    list->prev[0] = 0;

    list->free = 1;
}

void list_dtor(spisok* list)
{
    free(list->data);
    free(list->next);
    free(list->prev);

    list->data = NULL;
    list->next = NULL;
    list->prev = NULL;

    list->size = 0;
    list->free = 0;
}

void list_print(spisok* list)
{
    for(int i = 1; i != 0; i = list->next[i])
    {
        printf("%d -> ", list->data[i]);
    }
    printf("END");
}

int insert_after(spisok* list, int index, int value)
{
    LIST_VERIFY;

    if (list->prev[index] == POIZON)
        return POIZON;

    int new_index = list->free;
    list->free = list->next[new_index];

    list->data[new_index] = value;
    list->prev[new_index] = index;
    list->next[new_index] = list->next[index];

    list->prev[list->next[index]] = new_index;
    list->next[index] = new_index;

    LIST_VERIFY;

    return new_index;
}

int insert_before(spisok* list, int index, int value)
{
    return insert_after(list, list->prev[index], value);
}

int list_delete (spisok* list, int index)
{
    LIST_VERIFY;

    list->next[list->prev[index]] = list->next[index];
    list->prev[list->next[index]] = list->prev[index];

    list->data[index] = POIZON;
    list->prev[index] = POIZON;

    list->next[index] = list->free;
    list->free = index;

    LIST_VERIFY;

    return SUCCESS;
}


