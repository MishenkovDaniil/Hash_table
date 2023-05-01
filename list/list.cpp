#include <string.h>
#include "list.h"

// #include "ptr_check.h"

void set_error_bit (unsigned int *error, int bit)
{
    *error |= bit;
}

void list_ctor (List *list, int capacity, unsigned int *err)
{
    assert (list);

    if (capacity <= 0 || capacity > MAX_CAPACITY)
    {
        set_error_bit (err, LIST_INCORRECT_CAPACITY);
    }
    else
    {
        list->capacity = capacity;

        list_realloc (list, 0);

        list->size = INITIAL_SIZE;

        list->elems[NULL_ELEM].data = INITIAL_DATA;
        list->elems[NULL_ELEM].prev = list->elems[NULL_ELEM].next = INITIAL;

        list->free = 1;

        fill_list (list, INITIAL_SIZE, err);
    }

    #ifdef DEBUG 
    check_list (list, err);
    #endif
}

int list_insert (List *list, int put_place, list_elem_t value, unsigned int *err)
{
    assert (list);
    assert (err);

    #ifdef DEBUG 
    check_list (list, err);
    #endif

    list_realloc (list, list->capacity);
    int insert_index = list->free;
    int previous_free = list->free;


    if (put_place > MAX_CAPACITY)
    {
        set_error_bit (err, LIST_INCORRECT_INSERT_PLACE);
        #ifdef DEBUG 
        check_list (list, err);
        #endif

        list->free = previous_free;

        return POISON;
    }
    else if (list->elems[put_place].prev == EMPTY && put_place != NULL_ELEM)
    {
        set_error_bit (err, LIST_INSERT_ERROR);

        #ifdef DEBUG 
        check_list (list, err);
        #endif

        return POISON;
    }

    list->free = list->elems[previous_free].next;

    list->elems[insert_index].data = value;
    list->elems[insert_index].prev = put_place;
    list->elems[insert_index].next = list->elems[put_place].next;

    list->elems[put_place].next = insert_index;
    list->elems[list->elems[insert_index].next].prev = insert_index;

    (list->size)++;

    #ifdef DEBUG 
    check_list (list, err);
    #endif

    return insert_index;
}

// inline function??
list_elem_t list_remove (List *list, int remove_place, unsigned int *err)
{
// size capacity size_t
    if (!(remove_place > 0 && remove_place < list->capacity) || (list->elems[remove_place].prev == EMPTY))
    {
        fprintf (stderr, "REMOVE_ERROR: incorrect remove place");

        set_error_bit (err, LIST_INCORRECT_REMOVE_PLACE);
        list_dump (list, err);

        return POISON_DATA;
    }
    else if (list->size == 0)
    {
        fprintf (stderr, "REMOVE_ERROR: remove from empty list");

        set_error_bit (err, LIST_REMOVE_FROM_EMPTY_LIST);
        list_dump (list, err);

        return POISON_DATA;
    }

    #ifdef DEBUG 
    check_list (list, err);
    #endif

    list_elem_t return_value = list->elems[remove_place].data;

    list->elems[list->elems[remove_place].prev].next = list->elems[remove_place].next;
    list->elems[list->elems[remove_place].next].prev = list->elems[remove_place].prev;


    list->elems[remove_place].data = POISON_DATA;
    list->elems[remove_place].next = list->free;
    list->elems[remove_place].prev = EMPTY;

    list->free = remove_place;

    (list->size)--;

    #ifdef DEBUG 
    check_list (list, err);
    #endif

    return return_value;
}

void list_realloc (List *list, int previous_capacity, unsigned int *err)
{
    if (!(previous_capacity) || list->size >= list->capacity - 1)
    {
        list->capacity += previous_capacity;

        #ifdef CANARY_PROT
        list->elems = (List_elem *)realloc (list->elems, sizeof (List_elem) * list->capacity + sizeof (CANARY) * CANARY_NUM); //check if nullptr
        assert (list->elems);

        if (!(previous_capacity))
        {
            *(canary_t *)(list->elems) = CANARY;
        }

        list->elems = (List_elem *)((char *)list->elems + sizeof (CANARY));

        *(canary_t *)(list->elems + list->capacity) = CANARY;
        #else
        list->elems = (List_elem *)realloc (list->elems, list->capacity * sizeof (List_elem)); //check if nullptr
        assert (list->elems);
        #endif

        if (!(previous_capacity))
        {
            list->free = 1;
        }
        else
        {
            list->free = previous_capacity;
        }
        fill_list (list, previous_capacity, err);
    }
}

//headers in dump (h2)
//---------------------
int find_logic_number (List *list, int phys_index, unsigned int *err)
{
    printf ("the function will be working too long, do you really want to call it? (yes/no)");

    const int answer_size = 5;
    char status[answer_size] = {};

    scanf ("%s", status);

    if (strcasecmp (status, "yes") == 0)
    {
        int desired_index = 1;

        List_elem elem = list->elems[phys_index];

        if (elem.data != POISON_DATA && elem.prev != EMPTY)
        {
            while (elem.prev)
            {
                elem = list->elems[elem.prev];
                desired_index++;
            }
        }

        if (desired_index)
        {
            return desired_index;
        }

        printf ("this element is empty or null");
    }

    return 0;
}

int find_number (List *list, int phys_index, unsigned int *err)
{
    printf ("the function will be working too long, do you really want to call it? (yes/no)");

    const int answer_size = 5;
    char status[answer_size] = {};

    scanf ("%s", status);

    if (strcasecmp (status, "no") == 0)
    {
        int desired_index = 1;

        List_elem elem = list->elems[phys_index];

        if (elem.data != POISON_DATA && elem.prev != EMPTY)
        {
            while (elem.prev)
            {
                elem = list->elems[elem.prev];
                desired_index++;
            }
        }

        if (desired_index)
        {
            return desired_index;
        }

        printf ("this element is empty or null");
    }
    else
    {
        List new_list = {};
        new_list.elems[NULL_ELEM].data = POISON_DATA;
    }

    return 0;
}

int linearize_list (List *list, unsigned int *err)
{
    //no additional memory
    List_elem *temp_elems = (List_elem *)calloc (list->capacity, sizeof (List_elem));
    assert (temp_elems);

    if (temp_elems == nullptr)
    {
        fprintf (stderr, "ERROR: allocation failed");
        set_error_bit (err, LIST_ALLOCATION_FAIL);

        return *err;
    }

    int phys_index  = list->elems[NULL_ELEM].next;
    int logic_index = 1;

    while (logic_index <= list->size)
    {
        temp_elems[logic_index].data = list->elems[phys_index].data;
        temp_elems[logic_index].next = logic_index + 1;
        temp_elems[logic_index].prev = logic_index - 1;

        phys_index = list->elems[phys_index].next;
        logic_index++;
    }

    temp_elems[logic_index - 1].next = NULL_ELEM;
    temp_elems[NULL_ELEM].prev = logic_index - 1;
    temp_elems[NULL_ELEM].next = 1;
    temp_elems[NULL_ELEM].data = POISON_DATA;

    list->free = logic_index;

    while (logic_index < list->capacity)
    {
        temp_elems[logic_index].data = POISON_DATA;
        temp_elems[logic_index].next = logic_index + 1;
        temp_elems[logic_index].prev = EMPTY;

        logic_index++;
    }

    temp_elems[--logic_index].next = NULL_ELEM;

    free (list->elems);
    list->elems = temp_elems;

    #ifdef DEBUG 
    check_list (list, err);
    #endif

    return 0;
}

void fill_list (List *list, int start, unsigned int *err)
{
    for (int index = start; index < list->capacity; index++)
    {
        list->elems[index].data = POISON_DATA;
        list->elems[index].prev = EMPTY;

        if (index == list->capacity - 1)
        {
            list->elems[index].next = NULL_ELEM;
        }
        else
        {
            list->elems[index].next = index + 1;
        }
        if (index == 0)
        {
            list->elems[index].next = NULL_ELEM;
            list->elems[index].prev = NULL_ELEM;
        }
    }
}

void list_dtor (List *list, unsigned int *err)
{
    if (list != nullptr && list->elems != nullptr)
    {
        list_free (list);
    }
    else
    {
        #ifdef LIST_DEBUG
        printf ("\nnothing to dtor\n");
        #endif
    }
}

void list_free (List *list)
{
    #ifdef CANARY_PROT
    list->elems = (List_elem *)((char *)list->elems - sizeof (CANARY));
    #endif

    free (list->elems);

    list->elems = nullptr;
    free (list);
    list = nullptr;
}

void check_list (List *list, unsigned int *err)
{
    int index = NULL_ELEM;
    int counter = 0;

    do
    {
        if (!list)
        {
            set_error_bit (err, LIST_BAD_READ_LIST);
            break;
        }
        if (!(list->elems))
        {
            set_error_bit (err, LIST_BAD_READ_DATA);
            break;
        }
        if (list->size < 0)
        {
            set_error_bit (err, LIST_INCORRECT_SIZE);
        }
        if (list->capacity <= 0)
        {
            set_error_bit (err, LIST_INCORRECT_CAPACITY);
        }

        if (!(*err & LIST_INCORRECT_SIZE) && !(*err & LIST_INCORRECT_CAPACITY))
        {
            while (counter++ <= list->size)
            {
                if (list->elems[list->elems[index].next].prev != index)
                {
                    set_error_bit (err, LIST_PREV_NEXT_OP_ERR);
                }
                index = list->elems[index].next;
            }

            counter = 0;
            index = list->free;

            while (counter++ < list->capacity - list->size - 1)
            {
                if (list->elems[index].prev != EMPTY)
                {
                    set_error_bit (err, LIST_FREE_ELEM_NOT_EMPTY);
                }
            }
        }

        #ifdef CANARY_PROT
        if (list->left_canary != CANARY || list->right_canary != CANARY)
        {
            set_error_bit (err, LIST_VIOLATED_LIST);
        }
        if (   *(canary_t *)((char *)list->elems - sizeof (CANARY)) != CANARY
            || *(canary_t)(list->elems + list->capacity) != CANARY)
        {
            set_error_bit (err, LIST_VIOLATED_DATA);
        }
        #endif
    }while(0);

    if (*err)
    {
    list_dump (list, err);
    }
}

void list_dump (List *list, unsigned int *err)
{
    static int PNG_FILE_NUMBER = 1;

    FILE *list_graph = fopen ("list_graph", "w");
    assert (list_graph);

    fprintf (list_log, "<pre>\n");

    fprintf (list_log, "<details>");

    dump_list_members (list, err);

    fprintf (list_log, "</details>\n");

    make_graph (list, list_graph);
    fclose (list_graph);

    char cmd[100] = {};

    sprintf (cmd, "dot list_graph -T png -o dot%d.png", PNG_FILE_NUMBER);
    printf ("%s", cmd);
    system (cmd);


    fprintf (list_log, "<img src = dot%d.png>\n", PNG_FILE_NUMBER++);

    dump_list_errors  (list, err);

    fprintf (list_log, "\n\n\n\n\n");
}

void make_graph (List *list, FILE *list_graph)
{
    int idx = 0;
    fprintf (list_graph, "digraph {\n\tgraph [dpi = 100]\n\t"
                         "splines = \"ortho\";\n\t"
                         "rankdir = LR;\n\t"
                         "header [shape = record,  style = \"filled\", fillcolor = \"grey73\","
                         "label = \"idx\\n | data \\n | next \\n | prev \"];\n");
    while (idx < list->capacity)
    {
        if (list->elems[idx].prev == -1)
        {
            fprintf (list_graph, "\tlabel_%d [shape = record, style = \"filled\", fillcolor = \"salmon1\","
                                 "label = \"%d\\n | d[%x]\\n | n[%d]\\n | p[%d](empty)\"];\n ",
                                 idx, idx, list->elems[idx].data, list->elems[idx].next, list->elems[idx].prev);
        }
        else
        {
            fprintf (list_graph, "\tlabel_%d [shape = record, style = \"filled\", fillcolor = \"lightblue\","
                                 "label = \"%d\\n | d[%x]\\n | n[%d]\\n | P [%d]\"];\n ",
                                 idx, idx, list->elems[idx].data, list->elems[idx].next, list->elems[idx].prev);
        }
        idx++;
    }

    idx = 0;
    fprintf (list_graph, "\t{edge [style = \"invis\", arrowsize = 44, weight = 1000];\n\t");

    fprintf (list_graph, "header->label_%d;\n\t", idx);
    while (idx < list->capacity - 1)
    {
        fprintf (list_graph, "label_%d->", idx);
        idx++;
    }
    fprintf (list_graph, "label_%d;\n\t}\n", idx);

    fprintf (list_graph, "\tedge [color = \"purple\", weight = 10];\n\t");
    int counter = 0;
    idx = list->elems[NULL_ELEM].next;

    while (counter++ < list->size)
    {
        fprintf (list_graph, "\tlabel_%d->label_%d;\n", idx, list->elems[idx].next);
        idx = list->elems[idx].next;
    }
    fprintf (list_graph, "}");
}

void dump_list_members (List *list, unsigned int *err)
{
    #ifdef LIST_DEBUG
    fprintf (list_log, "%s at file %s, called at %s at %s, line %d\n", list->debug_info.func, list->debug_info.file,
    list->debug_info.call_func, list->debug_info.call_file, list->debug_info.call_line);
    #endif

    fprintf (list_log, "list[%p]\n", list);

    fprintf (list_log, "list head is %d\nlist tale is %d\n"
                       "list size id %d\nlist capacity id %d\n"
                       "list free is %d\nlist elems is [%p]\n",
                       list->elems[NULL_ELEM].next, list->elems[NULL_ELEM].prev, list->size, list->capacity, list->free, list->elems);
    dump_elems (list, err);
}

void dump_elems (List *list, unsigned int *err)
{
    for (int index = 0; index < list->capacity; index++)
    {
        fprintf (list_log, "idx[%d]\t data [%x]\t next is [%d]\t prev is [%d]\n",
                 index, list->elems[index].data, list->elems[index].next, list->elems[index].prev);
    }
}

void dump_list_errors (List *list, unsigned int *err)
{

#define log_error(__error_bit, __msg)        \
    if (*err & __error_bit)                  \
    {                                        \
        fprintf (list_log, __msg);           \
    }

    do
    {
        if (*err & LOG_FOPEN_FAIL)
        {
            fprintf (stderr, "opening of log file failed");
            break;
        }

        log_error(LIST_ALLOCATION_FAIL, "calloc failed");
        log_error(LIST_BAD_READ_LIST, "list is a bad ptr");
        log_error(LIST_BAD_READ_DATA, "list elems is a bad ptr");
        log_error(LIST_INCORRECT_CAPACITY, "capacity is incorrect (<=0)");
        log_error(LIST_PREV_NEXT_OP_ERR, "next element of previous is not equal to original");
        log_error(LIST_FREE_ELEM_NOT_EMPTY, "free element is not empty");
        log_error(LIST_VIOLATED_LIST, "access rights of list are invaded");
        log_error(LIST_VIOLATED_DATA, "access rights of list data are invaded");

    } while (0);

#undef log_error
}

void init_debug_info (List *list, const int call_line, const char *call_file, const char *call_func,
                      const char *file, const char *debug_func)
{
    #ifdef LIST_DEBUG 
    list->debug_info.call_line = call_line;
    list->debug_info.call_file = call_file;
    list->debug_info.call_func = call_func;

    list->debug_info.file = file;
    list->debug_info.func = debug_func;
    #endif 
}

void debug_list_ctor (List *list, int capacity, unsigned int *err,
                      const int call_line, const char *call_file, const char *call_func)
{
    init_debug_info (list, call_line, call_file, call_func, __FILE__,
                    "void list_ctor (List *list, int capacity, unsigned int *err = &ERRNO)");

    list_ctor (list, capacity, err);
}

void debug_list_dtor (List *list, unsigned int *err,
                      const int call_line, const char *call_file, const char *call_func)
{
    init_debug_info (list, call_line, call_file, call_func, __FILE__,
                    "void list_dtor (List *list, unsigned int *err = &ERRNO)");

    list_dtor (list, err);
}

int debug_list_insert (List *list, int put_place, list_elem_t value, unsigned int *err,
                       const int call_line, const char *call_file, const char *call_func)
{
    init_debug_info (list, call_line, call_file, call_func, __FILE__,
                    "int list_insert (List *list, int put_place, list_elem_t value, unsigned int *err = &ERRNO)");

    return list_insert (list, put_place, value, err);
}

list_elem_t debug_list_remove (List *list, int remove_place, unsigned int *err,
                               const int call_line, const char *call_file, const char *call_func)
{
    init_debug_info (list, call_line, call_file, call_func, __FILE__,
                    "list_elem_t list_remove (List *list, int remove_place, unsigned int *err = &ERRNO)");

    return list_remove (list, remove_place, err);
}

int debug_linearize_list (List *list, unsigned int *err,
                          const int call_line, const char *call_file, const char *call_func)
{
    init_debug_info (list, call_line, call_file, call_func, __FILE__,
                    "int linearize_list (List *list, unsigned int *err = &ERRNO);");

    return linearize_list (list, err);
}

int debug_find_logic_number (List *list, int phys_index, unsigned int *err,
                             const int call_line, const char *call_file, const char *call_func)
{
    init_debug_info (list, call_line, call_file, call_func, __FILE__,
                    "int find_logic_number (List *list, int phys_index, unsigned int *err = &ERRNO)");

    return find_logic_number (list, phys_index, err);
}

int debug_find_number (List *list, int phys_index, unsigned int *err,
                       const int call_line, const char *call_file, const char *call_func)
{
    init_debug_info (list, call_line, call_file, call_func, __FILE__,
                    "int find_number (List *list, int phys_index, unsigned int *err = &ERRNO)");

    return find_number (list, phys_index, err);
}