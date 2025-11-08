#include <TXLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>
#include <Windows.h>

//#define FINAL_VER //(для отключения верификации)

#include "spisok.h"

int main ()
{
    spisok list = {};
    list_ctor (&list, 4);
    insert_after(&list, 0, 10);
    insert_after(&list, 1, 1);
    // int ind = insert_after(&list, 2, 3);
    insert_after(&list, 1, 0);
    insert_after(&list, 2, 12);
    list_delete(&list, 3);
    list_realloc_down_hard(&list);

    // list.prev[list.next[ind]] = 1; //для демонстрации порчи
    // list.prev[ind] = 300;
//
//     insert_after(&list, 2, 30);
//     list_delete(&list, 2);
//     list_realloc_down_soft(&list);

    //check_time(&list); //сравнение времени

    unsigned int code = list_verif (&list);
    list_dump(code);

    return 0;
}

int list_ctor(spisok* list, int capacity)
{
    assert(list);

    list->data = (int*)calloc(capacity, sizeof(list->data[0])); //check return by KK
    list->next = (int*)calloc(capacity, sizeof(list->next[0]));
    list->prev = (int*)calloc(capacity, sizeof(list->prev[0]));
    list->capacity = capacity;

    for(int i = 0; i < capacity; i++)
    {
        list->data[i] = POIZON;
        list->next[i] = i + 1;
        list->prev[i] = POIZON;
    }
    list->next[capacity - 1] = POIZON;
    list->next[0] = 0;
    list->prev[0] = 0;

    list->free = 1;

    ON_NOT_FINAL(LIST_VERIFY_BEFORE)

    return 0;
}

int list_realloc(spisok* list)
{
    assert(list);

    int new_capacity = list->capacity * 2;

    list->data = (int*)realloc(list->data, new_capacity * sizeof(list->data[0]));
    list->next = (int*)realloc(list->next, new_capacity * sizeof(list->next[0]));
    list->prev = (int*)realloc(list->prev, new_capacity * sizeof(list->prev[0]));

    for (int i = list->capacity; i < new_capacity; i++)
    {
        list->data[i] = POIZON;
        list->prev[i] = POIZON;
        list->next[i] = i + 1;
    }

    list->next[new_capacity - 1] = POIZON;

    list->free = list->capacity;

    list->capacity = new_capacity;

    ON_NOT_FINAL(LIST_VERIFY_AFTER)

    return 0;
}

int list_realloc_down_soft(spisok* list)
{
    assert(list);

    ON_NOT_FINAL(LIST_VERIFY_BEFORE)

    int max_used_index = 0;
    for (int i = 0; i < list->capacity; i++)
    {
        if (list->prev[i] != POIZON && i > max_used_index)
            max_used_index = i;

    }

    int new_capacity = max_used_index + 1;

    if (new_capacity >= list->capacity)
        return SUCCESS;

    free(list->data + max_used_index + 1);
    free(list->next + max_used_index + 1);
    free(list->prev + max_used_index + 1);

    list->capacity = new_capacity;

    list->free = POIZON;
    for (int i = new_capacity - 1; i >= 0; i--)
    {
        if (list->prev[i] == POIZON && i != 0)
        {
            list->next[i] = list->free;
            list->free = i;
        }
    }

    ON_NOT_FINAL(LIST_VERIFY_AFTER)

    return SUCCESS;
}

void list_dtor(spisok* list)
{
    free(list->data);
    free(list->next);
    free(list->prev);

    list->data = NULL;
    list->next = NULL;
    list->prev = NULL;

    list->capacity = 0;
    list->free = 0;
}

int list_print(spisok* list) //verify by KK (DONE)
{
    assert(list);

    ON_NOT_FINAL(LIST_VERIFY_BEFORE)

    for(int i = 1; i != 0; i = list->next[i])
        printf("%d -> ", list->data[i]);

    printf("END");

    ON_NOT_FINAL(LIST_VERIFY_AFTER)

    return 0;
}

int insert_after(spisok* list, int index, int value)
{
    assert(list);

    ON_NOT_FINAL(LIST_VERIFY_BEFORE)

    if (list->prev[index] == POIZON || index == POIZON)
    {
        printf("ERROR: CANNOT INSERT (BAD ANHCOR)");
        return POIZON;
    }

    if (list->free == POIZON)
        list_realloc(list);


    int new_index = list->free;
    list->free = list->next[new_index];

    list->data[new_index] = value;
    list->prev[new_index] = index;
    list->next[new_index] = list->next[index];

    list->prev[list->next[index]] = new_index;
    list->next[index] = new_index;

    ON_NOT_FINAL(LIST_VERIFY_AFTER)

    return new_index;
}

int insert_before(spisok* list, int index, int value)
{
    assert(list);

    return insert_after(list, list->prev[index], value);
}

int list_delete (spisok* list, int index) //asserts by KK (DONE)
{
    assert(list);

    ON_NOT_FINAL(LIST_VERIFY_BEFORE)

    list->next[list->prev[index]] = list->next[index];
    list->prev[list->next[index]] = list->prev[index];

    list->data[index] = POIZON;
    list->prev[index] = POIZON;

    list->next[index] = list->free;
    list->free = index;

    ON_NOT_FINAL(LIST_VERIFY_AFTER)

    return SUCCESS;
}

int list_sort(spisok* list)
{
    assert(list);

    ON_NOT_FINAL(LIST_VERIFY_BEFORE)

    int* data_copy_arr = (int*)calloc(list->capacity, sizeof(data_copy_arr));
    if (data_copy_arr == NULL)
    {
        printf("Memory allocation failed in list_sort\n");
        return POIZON;
    }

    int n_data = 0;
    for(int i = 1; i != 0; i = list->next[i])
        data_copy_arr[n_data++] = list->data[i];

    for (int i = 0; i < n_data; i++)
    {
        list->data[i+1] = data_copy_arr[i];
        list->next[i+1] = i + 2;
        list->prev[i+1] = i;
    }
    list->next[n_data] = 0;
    list->next[0] = 1;
    list->prev[0] = n_data;

    if (n_data + 1 <= list->capacity)
    {
        list->free = n_data + 1;

        for(int i = n_data + 1; i < list->capacity; i++)
        {
            list->data[i] = POIZON;
            list->prev[i] = POIZON;
            list->next[i] = i + 1;
        }

        list->next[list->capacity - 1] = POIZON;
    }

    else
        list->free = POIZON;

    free(data_copy_arr);

    ON_NOT_FINAL(LIST_VERIFY_AFTER)

    return 0;
}

int list_realloc_down_hard(spisok* list)
{
    assert(list);

    ON_NOT_FINAL(LIST_VERIFY_BEFORE)

    #define FINAL_VER

    list_sort(list);
    list_realloc_down_soft(list);

    #undef FINAL_VER

    ON_NOT_FINAL(LIST_VERIFY_AFTER)

    return 0;
}

void check_time(spisok* list)
{
    LARGE_INTEGER start = {};
    LARGE_INTEGER end = {};

    QueryPerformanceCounter(&start);

    int curr_ind = 0;
    for (int i = 0; i < 10e7; i++)
    {
        curr_ind = insert_after(list, 0, 20);
        list_delete(list, curr_ind);
    }

    QueryPerformanceCounter(&end);

    double ticks = (double)(end.QuadPart - start.QuadPart);

    printf("Ticks: %.0lf\n", ticks);
}

