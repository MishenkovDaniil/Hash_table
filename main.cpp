#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <nmmintrin.h>
#include <time.h>

// #define HASH_DEBUG

#include "hash_table.h"
#include "parser/parser.h"
#include "hash_functions/hash_functions.h"
#include "root_mean_square/rms.h"

#define WORK_TIME_CHECK

static const char *CSV_FILENAME   = "data/histograms/hash.csv";
static const char *TEXT_FILENAME  = "data/texts/parsed_text.txt";
static const char *DUMP_FILENAME  = "data/dump_data/hash_dump";

static const size_t HASH_MMAP_CAPACITY = 4999;
static const size_t INIT_LIST_CAPACITY = 100;
static const size_t SEARCH_ITER_NUM    = 5000000;
static const size_t TIMETEST_HASH_IDX  = 6;
static const size_t CALC_NUM = 20;

static const int IS_SORTED = 0;
static const bool IS_NO_REPEATS = true;

static size_t (*hash_func[])(const char *) = {hash_ret_1,  hash_first_ch, 
                                       hash_strlen, hash_ch_sum, 
                                       hash_rotr,   hash_rotl,
                                       hash_crc64_opt,  nullptr};               /// hash_chc64 instead of hash_crc64_opt in no optimization case 
static const int HASH_FUNCTIONS_NUM = sizeof (hash_func) / sizeof (*hash_func);

void sort_text (Parsed_text *parsed_text, FILE *parsed_text_file);
void hash_comparison (Parsed_text *parsed_text, FILE *dump_file, size_t (**hash_func)(const char *), const bool is_no_repeats = false);
void load_hash_table (Hash_table *hash_table, Parsed_text *parsed_text, bool is_no_repeats);
void hash_table_work_time (Hash_table *hash_table, Parsed_text *parsed_text, const size_t calc_num, const size_t search_iter_num);
void print_elem_idxs (FILE *file, size_t elem_num);
size_t load_hash_maps (Hash_table *hash_maps, size_t (**hash_func)(const char *));
void hash_maps_dtor (Hash_table *hash_maps, const size_t hash_func_num);

int main ()
{   
    Parsed_text *parsed_text = parce_text (TEXT_FILENAME);

    FILE *dump_file = fopen (DUMP_FILENAME, "w");
    assert (dump_file);

    if (!IS_SORTED)
    {
        FILE *parsed_text_file = fopen ("data/texts/parsed_text.txt", "w");
        assert (parsed_text_file);
        sort_text (parsed_text, parsed_text_file);
        fclose (parsed_text_file);
    }

    hash_comparison (parsed_text, dump_file, hash_func, IS_NO_REPEATS);

    parced_text_dtor (parsed_text);

    return 0;
}

void hash_comparison (Parsed_text *parsed_text, FILE *dump_file, size_t (**hash_func)(const char *), const bool is_no_repeats)
{
    assert (parsed_text && dump_file);
    
    Hash_table hash_maps[HASH_FUNCTIONS_NUM] = {};

    FILE *csv_file = fopen (CSV_FILENAME, "w");
    assert (csv_file);

    const size_t hash_func_num = load_hash_maps (hash_maps, hash_func);
    
    for (size_t cur_hash_func_num = 0; cur_hash_func_num < hash_func_num; ++cur_hash_func_num)
    {
        load_hash_table (hash_maps + cur_hash_func_num, parsed_text, is_no_repeats);
        
        #ifdef HASH_DEBUG
        hash_table_dump (hash_maps + cur_hash_func_num, dump_file, csv_file);
        #endif 
    }

    #ifdef WORK_TIME_CHECK
    hash_table_work_time (hash_maps + TIMETEST_HASH_IDX, parsed_text, CALC_NUM, SEARCH_ITER_NUM);
    #endif 

    hash_maps_dtor (hash_maps, hash_func_num);
    
    print_elem_idxs (csv_file, HASH_MMAP_CAPACITY);
    fprintf (csv_file, "\n");

    fclose (csv_file);
}

void hash_maps_dtor (Hash_table *hash_maps, const size_t hash_func_num)
{
    for (size_t cur_hash_func_num = 0; cur_hash_func_num < hash_func_num; ++cur_hash_func_num)
    {
        hash_table_dtor (hash_maps + cur_hash_func_num);
    }
}

size_t load_hash_maps (Hash_table *hash_maps, size_t (**hash_func)(const char *))
{
    assert (hash_maps && hash_func);

    size_t hash_func_num = 0;

    while (*hash_func)
    {
        hash_table_ctor (hash_maps + hash_func_num, HASH_MMAP_CAPACITY, *hash_func, INIT_LIST_CAPACITY);
        hash_func++;
        hash_func_num++;
    }

    return hash_func_num;
}

void load_hash_table (Hash_table *hash_table, Parsed_text *parsed_text, bool is_no_repeats)
{
    assert (hash_table && parsed_text);

    for (size_t iter = 0; iter < parsed_text->size; ++iter)
    {
        if (is_no_repeats || !(hash_table_exists (hash_table, parsed_text->arr[iter].word)))
        {
            hash_table_insert (hash_table, parsed_text->arr[iter].word);
        }
    }
} 

void hash_table_work_time (Hash_table *hash_table, Parsed_text *parsed_text, const size_t calc_num, const size_t search_iter_num)
{
    assert (hash_table && parsed_text);

    double time_ms = 0;
    double *values = (double *)calloc (calc_num, sizeof (double));

    for (size_t cur_calc_num = 0; cur_calc_num < calc_num; ++cur_calc_num)
    {   
        clock_t t1 = clock();
        for (int iter = 0; iter < search_iter_num; ++iter)
        { 
            volatile List_elem *found = hash_table_find (hash_table, parsed_text->arr[iter % parsed_text->size].word);
        }

        clock_t t2 = clock ();

        values[cur_calc_num]  = (double) (1000 * (double)(t2 - t1)) / CLOCKS_PER_SEC;
        time_ms   += values[cur_calc_num];
    }

    printf ("time = %lf\n", time_ms / calc_num);
    printf ("rms = %lf\n", rms (values, calc_num, time_ms / calc_num));

    free (values);
}

void print_elem_idxs (FILE *file, size_t elem_num)
{
    assert (file);

    for (int idx = 0; idx < HASH_MMAP_CAPACITY; ++idx)
    {
        fprintf (file, "%d,", idx);
    }
}