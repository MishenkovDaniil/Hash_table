#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

static unsigned int ERRNO = 0;

typedef char list_elem_t[32];
typedef unsigned long long canary_t;

static const size_t INITIAL_SIZE = 0;
static const size_t INITIAL = 0;
static const int NULL_ELEM = 0;
static const int EMPTY = -1;
static const char POISON_DATA = -1;
static const int POISON = 0xDEADBEEF;
static const canary_t CANARY = 0xAB8EACAAAB8EACAA;
static const int CANARY_NUM = 2;
static const int MAX_CAPACITY = 100000;

static FILE *list_log = fopen ("list_log.html", "w");

enum errors
{
    LOG_FOPEN_FAIL             = 0x1 << 0,
    LIST_ALLOCATION_FAIL        = 0x1 << 1,
    LIST_BAD_READ_LIST          = 0x1 << 2,
    LIST_BAD_READ_DATA          = 0x1 << 3,
    LIST_INCORRECT_SIZE         = 0x1 << 4,
    LIST_INCORRECT_CAPACITY     = 0x1 << 5,
    LIST_INSERT_ERROR           = 0x1 << 6,
    LIST_INCORRECT_INSERT_PLACE = 0x1 << 7,
    LIST_INCORRECT_REMOVE_PLACE    = 0x1 << 8,
    LIST_REMOVE_FROM_EMPTY_LIST    = 0x1 << 9,
    LIST_PREV_NEXT_OP_ERR       = 0x1 << 10,
    LIST_FREE_ELEM_NOT_EMPTY    = 0x1 << 11,
    LIST_VIOLATED_LIST          = 0x1 << 12,
    LIST_VIOLATED_DATA          = 0x1 << 13,
};

struct List_elem
{
    list_elem_t data = "";
    int next = 0;
    int prev = 0;
};

struct List_debug
{
    const char *call_func = nullptr;
    const char *call_file = nullptr;
    const char *func      = nullptr;
    const char *file      = nullptr;
    int   call_line = 0;
};

struct List
{
    #ifdef CANARY_PROT
    canary_t left_canary = CANARY;
    #endif

    int free = 0;
    int size = 0;
    int capacity = 0;

    List_elem *elems = nullptr;

    #ifdef LIST_DEBUG
    List_debug debug_info = {};
    #endif

    #ifdef CANARY_PROT
    canary_t right_canary = CANARY;
    #endif
};


void list_ctor          (List *list, int capacity,                     unsigned int *err = &ERRNO);
void fill_list          (List *list, int start,                        unsigned int *err);
void list_dtor          (List *list,                                   unsigned int *err = &ERRNO);
int list_insert         (List *list, int put_place, char *value, unsigned int *err = &ERRNO);
void check_list          (List *list,                                   unsigned int *err);
void list_remove (List *list, int remove_place,                 unsigned int *err = &ERRNO);
void list_dump          (List *list,                                   unsigned int *err);
void dump_list_members  (List *list,                                   unsigned int *err);
void dump_elems         (List *list,                                   unsigned int *err);
void dump_list_errors   (List *list,                                   unsigned int *err);
void make_graph         (List *list, FILE *list_graph);
void list_realloc       (List *list, int previous_capacity,            unsigned int *err = &ERRNO);
int linearize_list      (List *list,                                   unsigned int *err = &ERRNO);
void list_free          (List *list);
int find_logic_number   (List *list, int phys_index, unsigned int *err = &ERRNO);
int find_number         (List *list, int phys_index, unsigned int *err = &ERRNO);
void set_error_bit      (unsigned int *error, int bit);

void init_debug_info (List *list, const int call_line, const char *call_file, const char *call_func,
                      const char *file, const char *func);
void debug_list_ctor          (List *list, int capacity, unsigned int *err,
                               const int call_line, const char *call_file, const char *call_func);
void debug_list_dtor          (List *list, unsigned int *err,
                               const int call_line, const char *call_file, const char *call_func);
int debug_list_insert         (List *list, int put_place, list_elem_t value, unsigned int *err,
                               const int call_line, const char *call_file, const char *call_func);
void debug_list_remove (List *list, int remove_place, unsigned int *err,
                               const int call_line, const char *call_file, const char *call_func);
int debug_linearize_list      (List *list, unsigned int *err,
                               const int call_line, const char *call_file, const char *call_func);
int debug_find_logic_number   (List *list, int phys_index, unsigned int *err,
                               const int call_line, const char *call_file, const char *call_func);
int debug_find_number         (List *list, int phys_index, unsigned int *err,
                               const int call_line, const char *call_file, const char *call_func);


#ifdef LIST_DEBUG

#define list_ctor(list, capacity, err)                                                          \
        debug_list_ctor (list, capacity, err, __LINE__, __FILE__, __PRETTY_FUNCTION__)
#define list_dtor(list, err)                                                                    \
        debug_list_dtor (list, err, __LINE__, __FILE__, __PRETTY_FUNCTION__)
#define list_insert(list, put_place, value, err)                                                \
        debug_list_insert (list, put_place, value, err, __LINE__, __FILE__, __PRETTY_FUNCTION__)
#define list_remove(list, remove_place, err)                                                    \
        debug_list_remove (list, remove_place, err, __LINE__, __FILE__, __PRETTY_FUNCTION__)
#define linearize_list(list, err)                                                               \
        debug_linearize_list (list, err, __LINE__, __FILE__, __PRETTY_FUNCTION__)
#define find_logic_number(list, phys_index, err)                                                \
        debug_find_logic_number (list, phys_index, err, __LINE__, __FILE__, __PRETTY_FUNCTION__)
#define find_number(list, phys_index, err)                                                      \
        debug_find_number (list, phys_index, err, __LINE__, __FILE__, __PRETTY_FUNCTION__)

#endif

#endif /* LIST_H */
