#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "list/list.h"
#include "hash.h"

extern "C" int m_strcmp (const char *str1, const char *str2);

Hash *Hash_ctor (size_t size, size_t (*hash_func)(char *string), size_t init_list_capacity)
{
    if (size < 1)
    {
        printf ("Error: incorrect hash size = %lu.\n", size);
        return nullptr;
    }

    Hash *hash = (Hash *)calloc (1, sizeof (Hash));
    assert (hash);

    hash->arr = (List **)calloc (size, sizeof (List *));
    assert (hash->arr);
    
    hash->size = size;
    hash->hash_func = hash_func;
    hash->init_list_capacity = init_list_capacity;

    return hash;
}

void Hash_insert (Hash *hash, char *string)
{
    assert (hash && string);

    size_t hash_value = ((hash->hash_func)(string)) % hash->size;

    List *insert_list = hash->arr[hash_value];
    
    if (!(insert_list))
    {
        hash->arr[hash_value] = insert_list = (List *)calloc (1, sizeof (List));

        list_ctor (insert_list, hash->init_list_capacity);
    }

    list_insert (insert_list, hash->arr[hash_value]->size, string);//hash->arr[hash_value]->size
}

void Hash_dtor (Hash *hash)
{
    if (!hash)
        return;

    clear_hash_arr (hash->arr, hash->size);
    free (hash->arr);
    hash->arr = nullptr;

    free (hash);
    hash = nullptr;
}

void clear_hash_arr (List **arr, size_t size)
{
    if (!(arr && size))
        return;

    for (size_t list_num = 0; list_num < size; ++list_num)
    {
        list_dtor (arr[list_num]);
        arr[list_num] = nullptr;    //
    }
}

void hash_dump (Hash *hash, FILE *dump_file, FILE *csv_file)
{
    assert (hash && dump_file);

    dump_hash (hash, dump_file);
    dump_hash_lists (hash->arr, hash->size, dump_file, csv_file);
}

void dump_hash (Hash *hash, FILE *dump_file)
{
    assert (hash && hash->arr && dump_file);

    fprintf (dump_file, "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n"
                        "\\\\\\\\\\\\\\\\\\\\\\\\HASH DUMP\\\\\\\\\\\\\\\\\\\\\\\\\n"
                        "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n");
    fprintf (dump_file, "Hash = %p;\nList array = %p;\nList array size = %lu;\nInit list capacity = %lu.\n\n",
                         hash, hash->arr, hash->size, hash->init_list_capacity);
}

void dump_hash_lists (List **arr, size_t arr_size, FILE *dump_file, FILE *csv_file)
{   
    assert (arr && arr_size && dump_file);

    fprintf (dump_file, "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n"
                        "\\\\\\\\\\\\\\\\\\\\\\\\LIST DUMP\\\\\\\\\\\\\\\\\\\\\\\\\n"
                        "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n");

    for (size_t list_num = 0; list_num < arr_size; ++list_num)
    {
        List *cur_list = arr[list_num];

        if (cur_list)
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
            fprintf (dump_file, "List %lu: nullptr;\n", list_num);
            fprintf (csv_file, "0,");
        }
        fflush (csv_file);
    }
    fprintf (csv_file, "\n");
}

bool Hash_exists (Hash *hash, char *string)
{
    assert (hash && string);
    
    return hash_find (hash, string) ? true : false;
}

List_elem *hash_find (Hash *hash, char *string)
{
    // static double time = 0;
    // static int num = 0;

    assert (hash && string);
    
    size_t hash_value = ((hash->hash_func)(string)) % hash->size;

    List *insert_list = hash->arr[hash_value];

    if (!(insert_list && insert_list->elems))
        return nullptr;

    List_elem *elem = &(insert_list->elems[insert_list->elems[NULL_ELEM].next]);
    List_elem *result = (List_elem *)0xDEADBEEF;

    while (1)
    {
    asm(".intel_syntax noprefix\n\t"
             "test %1, %1\n\t" 
             "je .end\n\t" 
             "mov rbx, 0xDEADBEEF\n\t" /// rbx = POISON_DATA; 
             "cmp rbx, %[data]\n\t" 
             "je .end\n\t" 
             "mov rsi, %[data]\n\t"
             "mov rdi, %[str]\n\t" 
             "call m_strcmp\n\t"  
             "test %%eax, %%eax\n\t"  
             "je .ret_elem\n\t" 
             "jmp .new_iter\n\t" 
        ".ret_elem:\n\t"    
             "mov %[res], %[elem]\n\t"
             "jmp .new_iter\n\t"
        ".end:\n\t"
             "mov %[res], 0\n\t"
        ".new_iter:\n\t" 
        ".att_syntax prefix"
            :[res] "+r"(result)
            :[data] "r"((elem->data)), [str] "r" (string), [elem] "r"(elem)
            :"rax", "rbx", "rsi", "rdi");

        if (result != (List_elem *)0xDEADBEEF)
        {
            return result;
        }

        elem = &(insert_list->elems[elem->next]);
    }
    
////     while (elem->data != nullptr && elem->data != POISON_DATA)
////     {
////         if (!(m_strcmp (elem->data, string)))
////         {
////             return elem;
////         }
////
////         elem = &(insert_list->elems[elem->next]);
////     }

////     return nullptr;
}