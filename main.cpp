#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

#include "hash.h"
#include "parser/parser.h"
#include "hash_functions/hash_functions.h"

const char *TEXT_FILENAME = "texts/war_and_peace.txt";
const char *DUMP_FILENAME = "hash_dump";
const size_t HASH_FUNCTIONS_NUM = 6;

void hash_comparison (Parsed_text *parsed_text, FILE *dump_file);


int main ()
{
    Parsed_text *parsed_text = parce_text (TEXT_FILENAME);
    fprintf (stderr, "size = %lu\n", parsed_text->size);
    fprintf (stderr, "size = %lu\n", parsed_text->capacity);

    FILE *dump_file = fopen (DUMP_FILENAME, "w");
    assert (dump_file);
    
    hash_comparison (parsed_text, dump_file);

    parced_text_dtor (parsed_text);

    return 0;
}

void hash_comparison (Parsed_text *parsed_text, FILE *dump_file)
{
    assert (parsed_text && dump_file);
    
    Hash **hash_mmaps = (Hash **)calloc (HASH_FUNCTIONS_NUM, sizeof (Hash *));

    hash_mmaps[0] = Hash_ctor (233, hash_ret_1, 100);
    hash_mmaps[1] = Hash_ctor (233, hash_first_ch, 100);
    hash_mmaps[2] = Hash_ctor (233, hash_strlen, 100);
    hash_mmaps[3] = Hash_ctor (233, hash_ch_sum, 100);
    hash_mmaps[4] = Hash_ctor (233, hash_rotr, 100);
    hash_mmaps[5] = Hash_ctor (233, hash_rotl, 100);

    for (size_t hash_func_num = 0; hash_func_num < HASH_FUNCTIONS_NUM; ++hash_func_num)
    {
        for (size_t iter = 0; iter < parsed_text->size; ++iter)
        {
            if (!(Hash_exists (hash_mmaps[hash_func_num], parsed_text->arr[iter])))
            {
                Hash_insert (hash_mmaps[hash_func_num], parsed_text->arr[iter]);
            }
        }

        hash_dump (hash_mmaps[hash_func_num], dump_file);

        Hash_dtor (hash_mmaps[hash_func_num]);
    }

    free (hash_mmaps);
}