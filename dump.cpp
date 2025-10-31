#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define DUMP_CPP

#include "spisok.h"

int GRAPH_COUNT = 0;

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

    for (int i = 0; i < list->size; i++)
    {
        if (list->prev[i] != POIZON)
        {
            if (list->prev[list->next[i]] != i)
                errors |= LIST_NEXT_NOT_EQV_PREV;
        }
    }

    bool* visited = (bool*)calloc(list->size, sizeof(bool));
    int current = list->next[0];
    int count = 0;

    while (current != 0 && current != POIZON && count < list->size)
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

    for (int i = 0; i < list->size; i++)
        visited[i] = false;

    current = list->prev[0];
    count = 0;

    while (current != 0 && current != POIZON && count < list->size)
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

void list_dump (const spisok* list, unsigned int error_code)
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
    fprintf(html_file, "<head>\n<title>List Dump</title></head>\n\n");
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
            fprintf(html_file, "<li>LIST_NEXT_NOT_EQV_PREV - Next and prev pointers are inconsistent</li>\n");

        fprintf(html_file, "</ul>\n");
    }

    else
        fprintf(html_file, "No errors detected\n");

    fprintf(html_file, "<h2>List Information</h2>\n");

    fprintf(html_file, "<h2>Data Arrays</h2>\n");
    fprintf(html_file, "<table>\n");
    fprintf(html_file, "<tr><th>Index</th><th>Data</th><th>Next</th><th>Prev</th><th>Status</th></tr>\n");

    for (int i = 0; i < list->size; i++)
    {
        const char* status = (list->prev[i] == POIZON) ? "FREE" : "USED";

        fprintf(html_file, "<tr><td>%d</td>", i);
        fprintf(html_file, "<td>%d</td>", list->data[i]);
        fprintf(html_file, "<td>%d</td>", list->next[i]);
        fprintf(html_file, "<td>%d</td>", list->prev[i]);
        fprintf(html_file, "<td>%s</td></tr>\n", status);
    }

    fprintf(html_file, "</table>\n");

    fprintf(html_file, "<h2>Graphical Representation</h2>\n");

    for (int i = 0; i < GRAPH_COUNT; i++)
    {
        char img_filename[STR_MAX_LEN] = "";
        sprintf(img_filename, "TEST%d.png", i);
        fprintf(html_file, "<h3>State %d</h3>\n", i);
        fprintf(html_file, "<img src='%s' alt='List graph state %d'>\n", img_filename, i);
    }

    fprintf(html_file, "</div>\n");

    fprintf(html_file, "</body>\n</html>\n");

    fclose(html_file);

    system(html_filename);
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
    fprintf(file, "node [shape=none, margin=0];\n");

    for (int i = 0; i < list->size; i++)
    {
        if (list->prev[i] != POIZON)
            make_table(list, file, i, i, "used");
        else
            make_table(list, file, i, i, "free");
    }

    make_order (list, file);

    make_arrows(list, file);

    fprintf(file, "}\n");
    fclose(file);

    make_graph(filename);
}

void make_table(spisok* list, FILE* file, int index, int type_count, const char* type)
{
    const char* fill_color = (strcmp(type, "used") == 0) ? "lightblue" : "lightgray";

    fprintf(file, "%s%d [label=<\n", type, index);
    fprintf(file, "\t<TABLE BORDER=\"1\" CELLBORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"4\" BGCOLOR=\"%s\">\n", fill_color);
    fprintf(file, "\t\t<TR><TD COLSPAN=\"2\" ALIGN=\"CENTER\"><B>INDEX: %d</B></TD></TR>\n", index);

    if (strcmp(type, "used") == 0)
    {
        fprintf(file, "\t\t<TR>\n");
        fprintf(file, "\t\t\t<TD PORT=\"next\">next: %d</TD>\n", list->next[index]);
        fprintf(file, "\t\t\t<TD PORT=\"prev\">prev: %d</TD>\n", list->prev[index]);
        fprintf(file, "\t\t</TR>\n");
        fprintf(file, "\t\t<TR><TD COLSPAN=\"2\" PORT=\"value\">value: %d</TD></TR>\n", list->data[index]);
    }
    else
    {
        fprintf(file, "\t\t<TR><TD COLSPAN=\"2\">FREE</TD></TR>\n");
        fprintf(file, "\t\t<TR><TD COLSPAN=\"2\">next: %d</TD></TR>\n", list->next[index]);
    }

    fprintf(file, "\t</TABLE>\n>];\n");
}

void make_order (spisok* list, FILE* file)
{
    fprintf(file, "{ rank = same; ");
    for (int i = 0; i < list->size; i++)
    {
        if (list->prev[i] != POIZON)
        {
            fprintf(file, "used%d; ", i);
        }
    }
    fprintf(file, "}\n");

    fprintf(file, "{ rank = same; ");
    for (int i = 0; i < list->size; i++)
    {
        if (list->prev[i] == POIZON)
            fprintf(file, "free%d; ", i);

    }
    fprintf(file, "}\n");

    fprintf(file, "used0 -> free%d [style=invis, weight = 1000];", list->free);

    int first_used = -1;
    int prev_used = -1;

    for (int i = 0; i < list->size; i++)
    {
        if (list->prev[i] != POIZON)
        {
            if (first_used == -1)
            {
                first_used = i;
                prev_used = i;
            }
            else
            {
                fprintf(file, "used%d -> used%d [style=invis, weight = 1000];\n", prev_used, i);
                prev_used = i;
            }
        }
    }
}

void make_arrows(const spisok* list, FILE* file)
{
    for (int i = 0; i < list->size; i++)
    {
        if (list->prev[i] != POIZON && list->next[i] != POIZON && list->next[i] < list->size)
            fprintf(file, "used%d -> used%d [color=red, constraint=false];\n", i, list->next[i]);
    }

    for (int i = 0; i < list->size; i++)
    {
        if (list->prev[i] != POIZON && list->prev[i] < list->size)
            fprintf(file, "used%d -> used%d [color=blue, style=dashed, constraint=false];\n", i, list->prev[i]);

    }

    int free_head = list->free;
    while (free_head != POIZON && free_head < list->size)
    {
        if (list->next[free_head] != POIZON && list->next[free_head] < list->size)
        {
            fprintf(file, "free%d -> free%d [color=green, style=dotted];\n",
                    free_head, list->next[free_head]);
        }
        free_head = list->next[free_head];
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
