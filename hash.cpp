#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "list/list.h"
#include "hash.h"

Hash *Hash_ctor (size_t size, size_t (*hash_func)(char *string), size_t list_capacity)
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
    hash->list_capacity = list_capacity;

    return hash;
}

void Hash_insert (Hash *hash, char *string)
{
    assert (hash && string);

    size_t num = ((hash->hash_func)(string)) % hash->size;

    if (!(hash->arr[num]))
    {
        hash->arr[num] = (List *)calloc (1, sizeof (List));

        list_ctor (hash->arr[num], hash->list_capacity);
    }

    list_insert (hash->arr[num], hash->arr[num]->size, string);
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

void hash_dump (Hash *hash, FILE *dump_file)
{
    assert (hash && dump_file);

    dump_hash (hash, dump_file);
    dump_hash_lists (hash->arr, hash->size, dump_file);
}

void dump_hash (Hash *hash, FILE *dump_file)
{
    assert (hash && hash->arr && dump_file);

    fprintf (dump_file, "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n"
                        "\\\\\\\\\\\\\\\\\\\\\\\\HASH DUMP\\\\\\\\\\\\\\\\\\\\\\\\\n"
                        "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n");
    fprintf (dump_file, "HASH = %p;\nLIST ARRAY = %p;\nLIST ARRAY SIZE = %lu;\nLIST CAPACITY = %lu.\n\n",
                         hash, hash->arr, hash->size, hash->list_capacity);
}

void dump_hash_lists (List **arr, size_t arr_size, FILE *dump_file)
{   
    assert (arr && arr_size && dump_file);

    fprintf (dump_file, "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n"
                        "\\\\\\\\\\\\\\\\\\\\\\\\LIST DUMP\\\\\\\\\\\\\\\\\\\\\\\\\n"
                        "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n");

    for (size_t list_num = 0; list_num < arr_size; ++list_num)
    {
        if (arr[list_num])
        {
            uint err = 0;
            check_list (arr[list_num], &err);
            list_dump (arr[list_num], &err);
            if (err)
            {
                fprintf (dump_file, "ERROR: err value is %u", err);
            }
        }
        else 
        {
            fprintf (dump_file, "list %d: nullptr;\n", list_num);
        }
    }
}
