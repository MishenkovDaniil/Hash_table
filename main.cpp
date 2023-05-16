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

#include "hash.h"
#include "parser/parser.h"
#include "hash_functions/hash_functions.h"
#include "root_mean_square/rms.h"

#define WORK_TIME_CHECK

static const char *CSV_FILENAME   = "data/histograms/hash.csv";
static const char *CSV_FILENAME_1 = "data/histograms/hash_1.csv";
static const char *CSV_FILENAME_2 = "data/histograms/hash_2.csv";
static const char *CSV_FILENAME_3 = "data/histograms/hash_3.csv";
static const char *CSV_FILENAME_4 = "data/histograms/hash_4.csv";
static const char *CSV_FILENAME_5 = "data/histograms/hash_5.csv";
static const char *CSV_FILENAME_6 = "data/histograms/hash_6.csv";
static const char *CSV_FILENAME_7 = "data/histograms/hash_7.csv";
static const char *TEXT_FILENAME  = "data/texts/parsed_text.txt";
static const char *DUMP_FILENAME  = "data/dump_data/hash_dump";

static const size_t HASH_FUNCTIONS_NUM = 7;
static const size_t HASH_MMAP_CAPACITY = 4999;
static const size_t INIT_LIST_CAPACITY = 100;
static const size_t SEARCH_ITER_NUM    = 5000000;
static const size_t TIMETEST_HASH_IDX  = 6;
static const size_t CALC_NUM = 20;

static const int IS_SORTED = 0;
void sort_text (Parsed_text *parsed_text, FILE *parsed_text_file);

void hash_comparison (Parsed_text *parsed_text, FILE *dump_file, const bool is_no_repeats = false);

int main ()
{   
    Parsed_text *parsed_text = parce_text (TEXT_FILENAME);

    FILE *dump_file = fopen (DUMP_FILENAME, "w");
    assert (dump_file);
    // // if (!IS_SORTED)
    // // {
    // //     FILE *parsed_text_file = fopen ("data/texts/parsed_text.txt", "w");
    // //     assert (parsed_text_file);
    // //     sort_text (parsed_text, parsed_text_file);
    // //     fclose (parsed_text_file);
    // // }

    hash_comparison (parsed_text, dump_file);

    parced_text_dtor (parsed_text);

    return 0;
}

void hash_comparison (Parsed_text *parsed_text, FILE *dump_file, const bool is_no_repeats)
{
    assert (parsed_text && dump_file);
    
    Hash **hash_mmaps = (Hash **)calloc (HASH_FUNCTIONS_NUM, sizeof (Hash *));
    assert (hash_mmaps);

    FILE *csv_file = fopen (CSV_FILENAME, "w");
    assert (csv_file);

    hash_mmaps[0]                   = Hash_ctor (HASH_MMAP_CAPACITY, hash_ret_1,     INIT_LIST_CAPACITY);
    hash_mmaps[1]                   = Hash_ctor (HASH_MMAP_CAPACITY, hash_first_ch,  INIT_LIST_CAPACITY);
    hash_mmaps[2]                   = Hash_ctor (HASH_MMAP_CAPACITY, hash_strlen,    INIT_LIST_CAPACITY);
    hash_mmaps[3]                   = Hash_ctor (HASH_MMAP_CAPACITY, hash_ch_sum,    INIT_LIST_CAPACITY);
    hash_mmaps[4]                   = Hash_ctor (HASH_MMAP_CAPACITY, hash_rotr,      INIT_LIST_CAPACITY);
    hash_mmaps[5]                   = Hash_ctor (HASH_MMAP_CAPACITY, hash_rotl,      INIT_LIST_CAPACITY);
    hash_mmaps[TIMETEST_HASH_IDX]   = Hash_ctor (HASH_MMAP_CAPACITY, hash_crc64_opt,     INIT_LIST_CAPACITY);
    
    for (size_t hash_func_num = 0; hash_func_num < HASH_FUNCTIONS_NUM; ++hash_func_num)
    {
        for (size_t iter = 0; iter < parsed_text->size; ++iter)
        {
            // if (is_no_repeats || !(Hash_exists (hash_mmaps[hash_func_num], parsed_text->arr[iter])))
            // {
                Hash_insert (hash_mmaps[hash_func_num], parsed_text->arr[iter].word);
            // }
        }
        
        #ifdef HASH_DEBUG
        hash_dump (hash_mmaps[hash_func_num], dump_file, csv_file);
        #endif 
    }

    #ifdef WORK_TIME_CHECK
    double time_ms = 0;
    double values[CALC_NUM] = {0};

    for (size_t calc_num = 0; calc_num < CALC_NUM; ++calc_num)
    {   
        clock_t t1 = clock();

        for (int iter = 0; iter < SEARCH_ITER_NUM; ++iter)
        {
            hash_find (hash_mmaps[TIMETEST_HASH_IDX], parsed_text->arr[iter % parsed_text->size].word);
        }

        clock_t t2 = clock ();

        values[calc_num]  = (double) (1000 * (double)(t2 - t1)) / CLOCKS_PER_SEC;
        time_ms   += values[calc_num];
    }

    printf ("time = %lf\n", time_ms / CALC_NUM);
    printf ("rsq = %lf\n", rms (values, CALC_NUM, time_ms / CALC_NUM));

    #endif 


    for (size_t hash_func_num = 0; hash_func_num < HASH_FUNCTIONS_NUM; ++hash_func_num)
    {
        Hash_dtor (hash_mmaps[hash_func_num]);
    }
    
    for (int idx = 0; idx < HASH_MMAP_CAPACITY; ++idx)
    {
        fprintf (csv_file, "%d,", idx);
    }

    fprintf (csv_file, "\n");

    fclose (csv_file);
    free (hash_mmaps);
}