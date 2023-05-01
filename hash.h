#ifndef HASH_H
#define HASH_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "list/list.h"

struct Hash 
{
    size_t size = 0;
    List **arr = nullptr;
    size_t (*hash_func)(char *string) = nullptr;
    size_t init_list_capacity = 0;
};

Hash *Hash_ctor (size_t size, size_t (*hash_func)(char *string), size_t init_list_capacity);
void Hash_insert (Hash *hash, char *string);
void Hash_dtor (Hash *hash);
void clear_hash_arr (List **arr, size_t size);
void hash_dump (Hash *hash, FILE *dump_file, FILE *csv_file);
void dump_hash (Hash *hash, FILE *dump_file);
void dump_hash_lists (List **arr, size_t arr_size, FILE *dump_file, FILE *csv_file);
bool Hash_exists (Hash *hash, char *string);
List_elem *hash_find (Hash *hash, char *string);

#endif /* HASH_H */