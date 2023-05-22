#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "list/list.h"
#include "hash_table.h"

extern "C" long m_strcmp (const char *str1, const char *str2);

static const unsigned int POISON_VAL = 0xDEADBEEF;

void hash_table_ctor (Hash_table *hash_table, size_t size, size_t (*hash_func)(const char *), size_t init_list_capacity)
{
    assert (hash_table);

    if (size < 1)
    {
        fprintf (stderr, "Error: incorrect hash size = %lu.\n", size);
        return;
    }

    hash_table->arr = (List *)calloc (size, sizeof (List));
    assert (hash_table->arr);

    for (size_t list_num = 0; list_num < size; ++list_num)
    {
        list_ctor (hash_table->arr + list_num, init_list_capacity);
    }
    
    hash_table->size = size;
    hash_table->hash_func = hash_func;
    hash_table->init_list_capacity = init_list_capacity;
}

void hash_table_insert (Hash_table *hash_table, char *string)
{
    assert (hash_table && string);

    size_t hash_value = ((hash_table->hash_func)(string)) % hash_table->size;

    List *insert_list = hash_table->arr + hash_value;

    list_insert (insert_list, hash_table->arr[hash_value].size, string);
}

void hash_table_dtor (Hash_table *hash_table)
{
    if (!hash_table)
        return;

    free (hash_table->arr);
    hash_table->arr = nullptr;
}

void hash_table_dump (Hash_table *hash_table, FILE *dump_file, FILE *csv_file)
{
    assert (hash_table && dump_file);

    dump_hash_table (hash_table, dump_file);
    dump_hash_table_lists (hash_table->arr, hash_table->size, dump_file, csv_file);
}

void dump_hash_table (Hash_table *hash_table, FILE *dump_file)
{
    assert (hash_table && hash_table->arr && dump_file);

    fprintf (dump_file, "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n"
                        "\\\\\\\\\\\\\\\\\\\\\\\\HASH TABLE DUMP\\\\\\\\\\\\\\\\\\\\\\\\\n"
                        "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n");
    fprintf (dump_file, "Hash_table = %p;\nList array = %p;\nList array size = %lu;\nInit list capacity = %lu.\n\n",
                         hash_table, hash_table->arr, hash_table->size, hash_table->init_list_capacity);
}

void dump_hash_table_lists (List *arr, size_t arr_size, FILE *dump_file, FILE *csv_file)
{   
    assert (arr && arr_size && dump_file);

    fprintf (dump_file, "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n"
                        "\\\\\\\\\\\\\\\\\\\\\\\\LIST DUMP\\\\\\\\\\\\\\\\\\\\\\\\\n"
                        "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n");

    for (size_t list_num = 0; list_num < arr_size; ++list_num)
    {
        List *cur_list = arr + list_num;

        if (cur_list->elems)
        {
            uint err = 0;
            check_list (cur_list, &err);
            // list_dump (cur_list, &err);

            if (err)
            {
                fprintf (dump_file, "Error: err value is %u", err);
            }

            fprintf (dump_file, "List %lu: size = %d\n\t\tcapacity = %d.\n", list_num, cur_list->size, cur_list->capacity);
            fprintf (csv_file, "%d,", cur_list->size);
        }
        else 
        {
            fprintf (dump_file, "List %lu: data = nullptr;\n", list_num);
            fprintf (csv_file, "0,");
        }
        fflush (csv_file);
    }
    fprintf (csv_file, "\n");
}

bool hash_table_exists (Hash_table *hash_table, char *string)
{
    assert (hash_table && string);
    
    return hash_table_find (hash_table, string) ? true : false;
}

List_elem *hash_table_find (Hash_table *hash_table, char *string)
{
    assert (hash_table && string);
    
    size_t hash_value = ((hash_table->hash_func)(string)) % hash_table->size;

    List insert_list = hash_table->arr[hash_value];

    if (!(insert_list.elems))
        return nullptr;

    List_elem *elem = &(insert_list.elems[insert_list.elems[NULL_ELEM].next]);

    while (elem->data[0] != POISON_DATA)
    {
        long temp;
        /// __asm       (".intel_syntax noprefix\n\t"
        ///             "mov %[temp], qword [%[data]]\n\t"
        ///             "sub %[temp], qword [%[str]]\n\t"
        ///             "jne .end_%=\n\t"
        ///             "mov %[temp], qword [%[data] + 8]\n\t"
        ///             "sub %[temp], qword [%[str] + 8]\n\t"
        ///             "jne .end_%=\n\t"
        ///             "mov %[temp], qword [%[data] + 16]\n\t"
        ///             "sub %[temp], qword [%[str] + 16]\n\t"
        ///             "jne .end_%=\n\t"
        ///             "mov %[temp], qword [%[data] + 24]\n\t"
        ///             "sub %[temp], qword [%[str] + 24]\n\t"
        ///         ".end_%=:\n\t"
        ///             ".att_syntax prefix"
        ///             : [temp] "=r" (temp)
        ///             : [str] "r" (string), [data] "r" (elem->data)
        ///             :
        ///             );
         __asm      (
                    "movq (%1), %0\n\t"
                    "subq (%2), %0\n\t"
                    "jne .end_%=\n\t"
                    "movq (%1), %0\n\t"
                    "subq (%2), %0\n\t"
                    "jne .end_%=\n\t"
                    "movq (%1), %0\n\t"
                    "subq (%2), %0\n\t"
                    "jne .end_%=\n\t"
                    "movq (%1), %0\n\t"
                    "subq (%2), %0\n\t"
                ".end_%=:\n\t"
                    : "=r" (temp)
                    : "r" (string), "r" (elem->data)
                    :
                    );

        
        /// if (!(m_strcmp(elem->data, string)))
        /// {
        ///     return elem;
        /// }
        if (!temp)
        {
            return elem;
        }

        elem = &(insert_list.elems[elem->next]);
    }

    return nullptr;
}