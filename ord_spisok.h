#ifndef ORD_SPISOK_H
#define ORD_SPISOK_H

#ifndef list_elem_t
#define list_elem_t int
#endif

struct node
{
    list_elem_t data;
    node* prev;
    node* next;
};

enum list_verif_error
{
    LIST_NO_ERRORS         = 0b000,
    LIST_BAD_MAIN_PTR      = 0b100,
    LIST_NEXT_CYCLED       = 0b010,
    LIST_NEXT_NOT_EQV_PREV = 0b001
};

struct graph_call_info
{
    const char* filename;
    const char* function;
    int line;
    const char* call_position;
    node* head_cpy;
};


const int MAX_GRAPH_CALLS = 100;

extern graph_call_info graph_calls[MAX_GRAPH_CALLS];
extern int GRAPH_COUNT;

#ifdef ORD_DUMP_CPP
    unsigned int list_verif_code = 0;
#else
    extern unsigned int list_verif_code;
#endif

#define LIST_VERIFY_HELPER(position)                          \
        list_verif_code = list_verif(HEAD);                   \
        if (GRAPH_COUNT < MAX_GRAPH_CALLS)                    \
        {                                                     \
            graph_calls[GRAPH_COUNT].filename = __FILE__;     \
            graph_calls[GRAPH_COUNT].function = __func__;     \
            graph_calls[GRAPH_COUNT].line = __LINE__;         \
            graph_calls[GRAPH_COUNT].call_position = position;\
            graph_calls[GRAPH_COUNT].head_cpy = copy_list(HEAD); \
        }                                                     \
        list_graph (HEAD, "dot_file.txt");                    \
        if(list_verif_code != LIST_NO_ERRORS)                 \
        {                                                     \
            list_dump(HEAD, list_verif_code);                 \
            return NULL;                                      \
        }


#define LIST_VERIFY_BEFORE LIST_VERIFY_HELPER("BEFORE")
#define LIST_VERIFY_AFTER LIST_VERIFY_HELPER("AFTER")

#ifndef FINAL_VER
#define ON_NOT_FINAL(code) code
#else
#define ON_NOT_FINAL(code)
#endif

node* create_node(list_elem_t data);
void list_dtor   (node* head);

node* insert_after (node* prev_node, list_elem_t data);
node* del_node     (node* del_node);
node* print_list   (node* head);

unsigned int list_verif (node* head);
void list_dump          (node* head, unsigned int error_code);

node* copy_list    (node* head);
void list_cpy_dtor (void);

void check_time (void);

void list_graph  (node* head, const char* filename);
void make_table  (node* current, FILE* file, int index);
void make_arrows (node* head, FILE* file);
void make_graph  (const char* filename);


#endif
