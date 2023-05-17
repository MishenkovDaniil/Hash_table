#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "list/list.h"
#include "hash_table.h"

//переименовать хэш в хэш-таблицу +
extern "C" int m_strcmp (const char *str1, const char *str2);

static const unsigned int POISON_VAL = 0xDEADBEEF;

Hash_table *hash_table_ctor (size_t size, size_t (*hash_func)(char *string), size_t init_list_capacity)
{
    if (size < 1)
    {
        printf ("Error: incorrect hash size = %lu.\n", size);
        return nullptr;
    }

    ///hash - переменная 
    Hash_table *hash_table = (Hash_table *)calloc (1, sizeof (Hash_table));
    assert (hash_table);

    hash_table->arr = (List *)calloc (size, sizeof (List));
    assert (hash_table->arr);

    for (size_t list_num = 0; list_num < size; ++list_num)
    {
        list_ctor (hash_table->arr + list_num, init_list_capacity);
    }
    
    hash_table->size = size;
    hash_table->hash_func = hash_func;
    hash_table->init_list_capacity = init_list_capacity;

    return hash_table;
}

void hash_table_insert (Hash_table *hash_table, char *string)
{
    assert (hash_table && string);

    size_t hash_value = ((hash_table->hash_func)(string)) % hash_table->size;

    List *insert_list = hash_table->arr + hash_value;
    
    // if (!(insert_list))
    // {
    //     hash_table->arr[hash_value] = insert_list = (List *)calloc (1, sizeof (List));

    //     list_ctor (&insert_list, hash_table->init_list_capacity);
    // }
///массив списков 
    list_insert (insert_list, hash_table->arr[hash_value].size, string);//hash->arr[hash_value]->size
}

void hash_table_dtor (Hash_table *hash_table)
{
    if (!hash_table)
        return;

    // clear_hash_table_arr (hash_table->arr, hash_table->size);
    free (hash_table->arr);
    hash_table->arr = nullptr;

    free (hash_table);
    hash_table = nullptr;
}

void clear_hash_table_arr (List **arr, size_t size)
{
    if (!(arr && size))
        return;

    for (size_t list_num = 0; list_num < size; ++list_num)
    {
        list_dtor (arr[list_num]);
        arr[list_num] = nullptr;    //
    }
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
    // List_elem *result = (List_elem *)POISON_VAL;
    
    // while (1)
    // {
    // asm volatile(".intel_syntax noprefix\n\t"
    //          "test %[data], %[data]\n\t" 
    //          "je .ret0\n\t" 
    //          "mov bl, 0xff\n\t" /// rbx = -1;POISON_DATA; 
    //          "cmp bl, byte [%[data]]\n\t" 
    //          "je .ret0\n\t" 
    //          "mov rdi, %[str]\n\t"
    //          "mov rsi, %[data]\n\t" 
    //          "call strcmp\n\t"  
    //          "test eax, eax\n\t"  
    //          "je .ret_elem\n\t" 
    //     ".ret0:\n\t"
    //          "mov %[res], 0\n\t"
    //          "jmp .end\n\t"
    //     ".ret_elem:\n\t"    
    //          "mov %[res], %[elem]\n\t"
    //     ".end: \n\t"
    //     ".att_syntax prefix"
    //         :[res] "=r"(result)
    //         :[data] "r"((elem->data)), [str] "r" (string), [elem] "r"(elem)
    //         :"rax", "rbx", "rcx", "rsi", "rdi", "cc", "memory");

    //     if (result != (List_elem *)POISON_VAL)
    //     {
    //         return result;
    //     }

    //     elem = &(insert_list->elems[elem->next]);
    // }    
    
    while (elem->data != nullptr && elem->data[0] != POISON_DATA)
    {
        if (!(strcmp (elem->data, string)))
        {
            return elem;
        }

        elem = &(insert_list.elems[elem->next]);
    }

    return nullptr;
}