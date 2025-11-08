#include <TXLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define DUMP_CPP

#include "spisok.h"

graph_call_info graph_calls[MAX_GRAPH_CALLS] = {};
int graph_count = 0;
const int STR_MAX_LEN = 100;

unsigned int list_verif (const spisok* list)
{
    unsigned int errors = LIST_NO_ERRORS;

    if (list == NULL)
        return errors | LIST_BAD_MAIN_PTR;

    if (list->data == NULL)
        errors |= LIST_BAD_DATA_PTR;

    if (list->next == NULL)
        errors |= LIST_BAD_NEXT_PTR;

    if (list->prev == NULL)
        errors |= LIST_BAD_PREV_PTR;

    if (errors != LIST_NO_ERRORS)
        return errors;

    if (list->next[0] == POIZON || list->prev[0] == POIZON)
        errors |= LIST_BAD_ANCHOR_ELEM;

    for (int i = 0; i < list->capacity; i++)
    {
        if (list->prev[i] != POIZON)
        {
            if (list->prev[list->next[i]] != i)
                errors |= LIST_NEXT_NOT_EQV_PREV;
        }
    }

    bool* visited = (bool*)calloc(list->capacity, sizeof(bool));
    int current = list->next[0];
    int count = 0;

    while (current != 0 && current != POIZON && count < list->capacity)
    {
        if (visited[current])
        {
            errors |= LIST_NEXT_CYCLED;
            break;
        }

        visited[current] = true;
        current = list->next[current];
        count++;
    }

    for (int i = 0; i < list->capacity; i++)
        visited[i] = false;

    current = list->prev[0];
    count = 0;

    while (current != 0 && current != POIZON && count < list->capacity)
    {
        if (visited[current])
        {
            errors |= LIST_PREV_CYCLED;
            break;
        }

        visited[current] = true;
        current = list->prev[current];
        count++;
    }

    free(visited);
    return errors;
}

void list_dump (unsigned int error_code)
{
    int dump_count = 0;

    char html_filename[STR_MAX_LEN];
    sprintf(html_filename, "dump%d.html", dump_count++);

    FILE* html_file = fopen(html_filename, "w");
    if (!html_file)
    {
        printf("Failed to open HTML dump file\n");
        return;
    }

    fprintf(html_file, "<html>\n");
    fprintf(html_file, "<head><title>List Dump</title></head>\n");
    fprintf(html_file, "<body>\n");

    fprintf(html_file, "<h1>List Structure Dump</h1>\n");

    fprintf(html_file, "<h2>Error Information</h2>\n");
    fprintf(html_file, "<h3>Error code: 0x%04X</h3>\n", error_code);

    if (error_code != LIST_NO_ERRORS)
    {
        fprintf(html_file, "Detected errors:\n");
        fprintf(html_file, "<ul>\n");

        if (error_code & LIST_BAD_MAIN_PTR)
            fprintf(html_file, "<li>LIST_BAD_MAIN_PTR - Invalid list pointer</li>\n");
        if (error_code & LIST_BAD_DATA_PTR)
            fprintf(html_file, "<li>LIST_BAD_DATA_PTR - Invalid data array pointer</li>\n");
        if (error_code & LIST_BAD_NEXT_PTR)
            fprintf(html_file, "<li>LIST_BAD_NEXT_PTR - Invalid next array pointer</li>\n");
        if (error_code & LIST_BAD_PREV_PTR)
            fprintf(html_file, "<li>LIST_BAD_PREV_PTR - Invalid prev array pointer</li>\n");
        if (error_code & LIST_BAD_ANCHOR_ELEM)
            fprintf(html_file, "<li>LIST_BAD_ANCHOR_ELEM - Invalid anchor element</li>\n");
        if (error_code & LIST_NEXT_CYCLED)
            fprintf(html_file, "<li>LIST_NEXT_CYCLED - Cycle detected in next pointers</li>\n");
        if (error_code & LIST_PREV_CYCLED)
            fprintf(html_file, "<li>LIST_PREV_CYCLED - Cycle detected in prev pointers</li>\n");
        if (error_code & LIST_NEXT_NOT_EQV_PREV)
            fprintf(html_file, "<li>LIST_NEXT_NOT_EQV_PREV - Next and prev pointers are not eqvivalent</li>\n");

        fprintf(html_file, "</ul>\n");
    }

    else
        fprintf(html_file, "No errors detected\n");

    fprintf(html_file, "<pre>");
    for (int i = 0; i < graph_count; i++)
    {
        if(graph_calls[i].filename != NULL)
            fprintf(html_file, "<h3>%s %s() in %s:%d</h3>\n", graph_calls[i].call_position, graph_calls[i].function, graph_calls[i].filename, graph_calls[i].line);
        else
            fprintf(html_file, "<h3>Emergensy exit</h3>\n");

        #define FILL_INFO(name, info)                \
            fprintf(html_file, "%6s", name);         \
            for (int index = 0; index < graph_calls[i].list_cpy->capacity; index++) \
                fprintf(html_file, " %4d", info);    \
            fprintf(html_file, "\n");

        FILL_INFO("Index:", index)
        FILL_INFO("Data:", graph_calls[i].list_cpy->data[index])
        FILL_INFO("Next:", graph_calls[i].list_cpy->next[index])
        FILL_INFO("Prev:", graph_calls[i].list_cpy->prev[index])

        #undef FILL_INFO

        char img_filename[STR_MAX_LEN] = "";
        sprintf(img_filename, "TEST%d.png", i);
        fprintf(html_file, "<img src='%s'>\n", img_filename);
    }

    fprintf(html_file, "</pre></body>\n</html>\n");

    graph_calls_dtor();

    fclose(html_file);

    system(html_filename);
}

void list_info_copy(spisok* list, spisok* list_cpy)
{
    assert(list);
    assert(list_cpy);

    if (list_cpy->data != NULL)
        free(list_cpy->data);
    if (list_cpy->next != NULL)
         free(list_cpy->next);
    if (list_cpy->prev != NULL)
        free(list_cpy->prev);

    list_cpy->data = (int*)calloc(list->capacity, sizeof(int));
    list_cpy->next = (int*)calloc(list->capacity, sizeof(int));
    list_cpy->prev = (int*)calloc(list->capacity, sizeof(int));

    list_cpy->capacity = list->capacity;
    list_cpy->free = list->free;

    for (int i = 0; i < list->capacity; i++)
    {
        list_cpy->data[i] = list->data[i];
        list_cpy->next[i] = list->next[i];
        list_cpy->prev[i] = list->prev[i];
    }
}

void graph_calls_dtor(void)
{
    for (int i = 0; i < MAX_GRAPH_CALLS; i++)
    {
        if (graph_calls[i].list_cpy != NULL)
        {
            free(graph_calls[i].list_cpy->data);
            free(graph_calls[i].list_cpy->next);
            free(graph_calls[i].list_cpy->prev);
            free(graph_calls[i].list_cpy);
        }
    }
}

void list_graph(spisok* list, const char* filename)
{
    FILE* file = fopen(filename, "w");
    if (!file)
    {
        printf("Failed to open dump file");
        return;
    }

    fprintf(file, "digraph Graph {\n");
    fprintf(file, "rankdir=HR\n");
    fprintf(file, "nodesep=0.5;\n");
    fprintf(file, "ranksep=0.8;\n");
    fprintf(file, "splines=ortho;\n");
    fprintf(file, "node [shape=diamond, margin=0.1, width=1, style=filled, fillcolor=red];\n");
    for (int i = 0; i < list->capacity; i++)
    {
        if (list->prev[i] != POIZON)
            make_table(list, file, i, "used");
        else
            make_table(list, file, i, "free");
    }

    make_order (list, file);

    make_arrows(list, file);

    fprintf(file, "}\n");
    fclose(file);

    make_graph(filename);
}

void make_table(spisok* list, FILE* file, int index, const char* type)
{
    if (strcmp(type, "used") == 0)
    {
    fprintf(file, "%s%d [label=\"{{index: %d}|{next: %d|prev: %d}|{value: %d}}\"", type, index, index, list->next[index], list->prev[index], list->data[index]);
    fprintf(file, "shape=Mrecord, fillcolor=lightblue]\n");
    }

    else
    {
    fprintf(file, "%s%d [label=\"{{index: %d}|{FREE}|{next: %d}}\",", type, index, index, list->next[index]);
    fprintf(file, "shape=Mrecord, fillcolor=lightgray]\n");
    }
}

void make_order(spisok* list, FILE* file)
{
    fprintf(file, "{ rank = same; ");
    for (int i = 0; i < list->capacity; i++)
    {
        if (list->prev[i] != POIZON)
            fprintf(file, "used%d; ", i);
        else
            fprintf(file, "free%d; ", i);
    }
    fprintf(file, "}\n");

    fprintf(file, "used0");
    for (int i = 1; i < list->capacity; i++)
    {
        fprintf(file, " -> ");
        if (list->prev[i] != POIZON)
            fprintf(file, "used%d", i);
        else
            fprintf(file, "free%d", i);
    }
    fprintf(file, " [style=invis, weight=1000];\n");
}

void make_arrows(const spisok* list, FILE* file)
{
    for (int i = 0; i < list->capacity; i++)
    {
        if (list->prev[i] != POIZON && list->next[i] != 0)
            fprintf(file, "used%d -> used%d [color=red, constraint=false];\n", i, list->next[i]);
    }

    for (int i = 0; i < list->capacity; i++)
    {
        if (list->prev[i] != POIZON && list->prev[i] != 0 && list->prev[i] != POIZON)
            fprintf(file, "used%d -> used%d [color=blue, constraint=false];\n", i, list->prev[i]);
    }

    int free_head = list->free;
    while (free_head != POIZON && free_head < list->capacity)
    {
        if (list->next[free_head] != POIZON && list->next[free_head] < list->capacity)
        {
            fprintf(file, "free%d -> free%d [color=green, style=dotted, constraint=false];\n",
                    free_head, list->next[free_head]);
        }
        free_head = list->next[free_head];
    }
}

void make_graph(const char* filename)
{
    char programm[STR_MAX_LEN] = "";
    sprintf(programm, "dot.exe %s -T png -o TEST%d.png", filename, graph_count++);

    int status = system(programm);

    if (status != 0)
        printf("Error of creation image in system function\n");
}
