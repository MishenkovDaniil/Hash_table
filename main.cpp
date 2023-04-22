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

int main ()
{
    Parsed_text *parsed_text = parce_text (TEXT_FILENAME);
    fprintf (stderr, "size = %lu\n", parsed_text->size);
    fprintf (stderr, "size = %lu\n", parsed_text->capacity);

    Hash *hash = Hash_ctor (233, hash_1, 100);
    
    FILE *dump_file = fopen ("hash_dump", "w");
    assert (dump_file);
    
    for (size_t iter = 0; iter < parsed_text->size; ++iter)
    {
        Hash_insert (hash, parsed_text->arr[iter]);
    }

    hash_dump (hash, dump_file);

    Hash_dtor (hash);

    parced_text_dtor (parsed_text);
    
    return 0;
}