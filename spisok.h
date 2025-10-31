#ifndef SPISOK_H
#define SPISOK_H

const int POIZON  = -1;
const int SUCCESS =  0;

#ifdef DUMP_CPP

unsigned int list_verif_code = 0;

#else

extern unsigned int list_verif_code;

#endif

struct spisok
{
    int* data;
    int* next;
    int* prev;
    int size;
    int free;
};

enum list_verif_error
{
    LIST_NO_ERRORS         = 0b00000000,
    LIST_BAD_MAIN_PTR      = 0b10000000,
    LIST_BAD_DATA_PTR      = 0b01000000,
    LIST_BAD_NEXT_PTR      = 0b00100000,
    LIST_BAD_PREV_PTR      = 0b00010000,
    LIST_BAD_ANCHOR_ELEM   = 0b00001000,
    LIST_NEXT_CYCLED       = 0b00000100,
    LIST_PREV_CYCLED       = 0b00000010,
    LIST_NEXT_NOT_EQV_PREV = 0b00000001
};

#define LIST_VERIFY                                      \
        list_verif_code = list_verif(list);              \
        if(list_verif_code != LIST_NO_ERRORS)            \
        {                                                \
            list_graph (list, "test1.txt");                \
            list_dump(list, list_verif_code);            \
            return POIZON;                               \
        }

void list_graph  (spisok* list, const char* filename);
void make_table  (spisok* list, FILE* file, int index, int type_count, const char* type);
void make_order (spisok* list, FILE* file);
void make_arrows (const spisok* list, FILE* file);
void make_graph  (const char* filename);

unsigned int list_verif (const spisok* list);
void list_dump          (const spisok* list, unsigned int error_code);

void list_ctor    (spisok* list, int size);
void list_dtor    (spisok* list);
int insert_after  (spisok* list, int index, int value);
int insert_before (spisok* list, int index, int value);
int list_delete   (spisok* list, int index);
void list_print(spisok* list);

#endif
