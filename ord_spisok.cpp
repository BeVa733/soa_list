#include <TXLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#define list_elem_t int
#define FINAL_VER

#include "ord_spisok.h"

node* HEAD = NULL;

int main ()
{
    HEAD = create_node(10);
//     node* current_node = insert_after(HEAD, 20);
//     insert_after(current_node, 30);
//     node* node_for_del = insert_after (current_node, 25);
//     // print_list(HEAD);
//     del_node(node_for_del);
//     // print_list(HEAD);
//
//     unsigned int code = list_verif (HEAD);
//
//     list_dump(HEAD, code);

    check_time();

    list_dtor(HEAD);

    return 0;
}

node* create_node(list_elem_t data)
{
    node* new_node = (node*)calloc(1, sizeof(node));
    new_node->data = data;
    new_node->prev = NULL;
    new_node->next = NULL;

    return new_node;
}

void list_dtor(node* head)
{
    assert(head);

    node* fried = NULL;

    while (head != NULL)
    {
        fried = head;
        head = head->next;
        free(fried);
    }
}

node* insert_after(node* prev_node, list_elem_t data)
{
    assert(prev_node);

    ON_NOT_FINAL(LIST_VERIFY_BEFORE)

    if (prev_node == NULL)
    {
        printf("Error: prev elem is NULL\n");
        return NULL;
    }

    node* new_node = create_node(data);

    new_node->prev = prev_node;
    new_node->next = prev_node->next;

    prev_node->next = new_node;

    if (new_node->next != NULL)
        (new_node->next)->prev = new_node;

    ON_NOT_FINAL(LIST_VERIFY_AFTER)

    return new_node;
}

node* del_node (node* del_node_ptr)
{
    assert(del_node_ptr);

    ON_NOT_FINAL(LIST_VERIFY_BEFORE)

    if (del_node_ptr->prev != NULL)
        del_node_ptr->prev->next = del_node_ptr->next;

    if (del_node_ptr->next != NULL)
        del_node_ptr->next->prev = del_node_ptr->prev;

    free(del_node_ptr);

    ON_NOT_FINAL(LIST_VERIFY_AFTER)

    return NULL;
}

node* print_list(node* head)
{
    assert(head);

    ON_NOT_FINAL(LIST_VERIFY_BEFORE)

    node* printed = head;

    while (printed != NULL)
    {
        printf("%d -> ", printed->data);
        printed = printed->next;
    }
    printf("END\n");

    ON_NOT_FINAL(LIST_VERIFY_BEFORE)

    return NULL;
}

unsigned int list_verif(node* head)
{
    unsigned int errors = LIST_NO_ERRORS;

    if (head == NULL)
        return errors | LIST_BAD_MAIN_PTR;

    node* slow = head;
    node* fast = head;
    bool has_cycle = false;

    while (fast != NULL && fast->next != NULL)
    {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast)
        {
            errors |= LIST_NEXT_CYCLED;
            has_cycle = true;
            break;
        }
    }

    node* current = head;
    node* prev = NULL;

    while (current != NULL && !has_cycle)
    {
        if (current->prev != prev)
            errors |= LIST_NEXT_NOT_EQV_PREV;

        if (current->next != NULL && current->next->prev != current)
            errors |= LIST_NEXT_NOT_EQV_PREV;

        prev = current;
        current = current->next;
    }

    return errors;
}

void check_time(void)
{
    LARGE_INTEGER start = {};
    LARGE_INTEGER end = {};

    QueryPerformanceCounter(&start);

    node* current = NULL;
    for (int i = 0; i < 10e7; i++)
    {
        current = insert_after(HEAD, 20);
        del_node(current);
    }

    QueryPerformanceCounter(&end);

    double ticks = (double)(end.QuadPart - start.QuadPart);

    printf("Ticks: %.0lf\n", ticks);
}


