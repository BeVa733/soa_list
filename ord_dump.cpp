#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define ORD_DUMP_CPP

#include "ord_spisok.h"

graph_call_info graph_calls[MAX_GRAPH_CALLS] = {};
int GRAPH_COUNT = 0;
const int STR_MAX_LEN = 100;

extern node* HEAD;

void list_dump(node* head, unsigned int error_code)
{
    static int dump_count = 0;
    char html_filename[STR_MAX_LEN];
    sprintf(html_filename, "ord_dump%d.html", dump_count++);

    FILE* html_file = fopen(html_filename, "w");
    if (!html_file)
    {
        printf("Failed to open HTML dump file\n");
        return;
    }

    fprintf(html_file, "<html>\n");
    fprintf(html_file, "<head>\n<title>Ordered List Dump</title>\n");
    fprintf(html_file, "</head>\n\n");
    fprintf(html_file, "<body>\n");

    fprintf(html_file, "<h1>Ordinary List Dump</h1>\n");

    fprintf(html_file, "<h2>Error Information</h2>\n");
    fprintf(html_file, "<h3>Error code: 0x%04X</h3>\n", error_code);

    if (error_code != LIST_NO_ERRORS)
    {
        fprintf(html_file, "<div class=\"error\">Detected errors:</div>\n");
        fprintf(html_file, "<ul>\n");

        if (error_code & LIST_BAD_MAIN_PTR)
            fprintf(html_file, "<li>LIST_BAD_MAIN_PTR - Invalid list pointer</li>\n");
        if (error_code & LIST_NEXT_CYCLED)
            fprintf(html_file, "<li>LIST_NEXT_CYCLED - Cycle detected in next pointers</li>\n");
        if (error_code & LIST_NEXT_NOT_EQV_PREV)
            fprintf(html_file, "<li>LIST_NEXT_NOT_EQV_PREV - Next and prev pointers are not equivalent</li>\n");

        fprintf(html_file, "</ul>\n");
    }
    else
        fprintf(html_file, "No errors detected\n");

    fprintf(html_file, "<h2>List Information</h2>\n");

    fprintf(html_file, "<pre>");

    for (int i = 0; i < GRAPH_COUNT; i++)
    {
        if(graph_calls[i].filename != NULL)
            fprintf(html_file, "<h3>%s %s() in %s:%d</h3>\n", graph_calls[i].call_position, graph_calls[i].function, graph_calls[i].filename, graph_calls[i].line);
        else
            fprintf(html_file, "<h3>Emergensy exit</h3>\n");

        node* current = graph_calls[i].head_cpy;

        fprintf(html_file, "%6s", "Data:");
        node* temp = current;
        while(temp != NULL)
        {
            fprintf(html_file, "%10d", temp->data);
            temp = temp->next;
        }
        fprintf(html_file, "\n");

        fprintf(html_file, "%6s", "Next:");
        temp = current;
        while(temp != NULL)
        {
            fprintf(html_file, "%10p", temp->next);
            temp = temp->next;
        }
        fprintf(html_file, "\n");

        fprintf(html_file, "%6s", "Prev:");
        temp = current;
        while(temp != NULL)
        {
            fprintf(html_file, "%10p", temp->prev);
            temp = temp->next;
        }
        fprintf(html_file, "\n");

        char img_filename[STR_MAX_LEN] = "";
        sprintf(img_filename, "TEST%d.png", i);
        fprintf(html_file, "<img src='%s'>\n", img_filename);
    }

    fprintf(html_file, "</pre></body>\n</html>\n");

    list_cpy_dtor();

    fclose(html_file);

    system(html_filename);
}

node* copy_list(node* head)
{
    if (head == NULL) return NULL;

    node* new_head = create_node(head->data);
    node* current_old = head->next;
    node* current_new = new_head;

    while (current_old != NULL)
    {
        node* new_node = create_node(current_old->data);
        new_node->prev = current_new;
        current_new->next = new_node;
        current_new = new_node;
        current_old = current_old->next;
    }

    return new_head;
}

void list_cpy_dtor(void)
{
    for (int i = 0; i < GRAPH_COUNT; i++)
    {
        if (graph_calls[i].head_cpy != NULL)
            list_dtor(graph_calls[i].head_cpy);
    }
}

void list_graph(node* head, const char* filename)
{
    assert(head);
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

    int index = 0;
    node* current = head;
    while (current != NULL)
    {
        make_table(current, file, index++);
        current = current->next;
    }

    make_arrows(head, file);

    fprintf(file, "}\n");
    fclose(file);

    make_graph(filename);
}

void make_table(node* current, FILE* file, int index)
{
    fprintf(file, "data%d [label=\"{{index: %d}|{next: %p|prev: %p}|{value: %d}}\"", index, index, current->next, current->prev, current->data);
    fprintf(file, "shape=Mrecord, fillcolor=orange]\n");
}

void make_arrows(node* head, FILE* file)
{
    node* printed = head;
    node* current = head;
    int index = 0;

    fprintf(file, "{ rank = same; ");
    while(current != NULL)
    {
        fprintf(file, "data%d; ", index++);
        current = current->next;
    }
    fprintf(file, "}\n");

    index = 0;
    while (printed->next != NULL)
    {
        fprintf(file, "data%d -> data%d\n", index, index+1);
        index++;
        printed = printed->next;
    }
}

void make_graph(const char* filename)
{
    char programm[STR_MAX_LEN] = "";
    sprintf(programm, "dot.exe %s -T png -o TEST%d.png", filename, GRAPH_COUNT++);

    int status = system(programm);

    if (status != 0)
        printf("Error of creation image in system function\n");
}
