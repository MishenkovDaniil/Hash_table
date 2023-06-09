#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "list/list.h"

struct Hash_table 
{
    size_t size = 0;
    List *arr = nullptr;
    size_t (*hash_func)(const char *) = nullptr;
    size_t init_list_capacity = 0;
};

void hash_table_ctor (Hash_table *hash_table, size_t size, size_t (*hash_func)(const char *), size_t init_list_capacity);
void hash_table_insert (Hash_table *hash, char *string);
bool hash_table_exists (Hash_table *hash, char *string);
void hash_table_dump (Hash_table *hash, FILE *dump_file, FILE *csv_file);
void hash_table_dtor (Hash_table *hash);
List_elem *hash_table_find (Hash_table *hash, char *string);

void dump_hash_table (Hash_table *hash, FILE *dump_file);
void dump_hash_table_lists (List *arr, size_t arr_size, FILE *dump_file, FILE *csv_file);

#endif /* HASH_TABLE_H */