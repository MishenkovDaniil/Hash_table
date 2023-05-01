#ifndef PARSER_H
#define PARSER_H

#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static const size_t START_SIZE = 100;
static const size_t MAX_STR_SIZE = 1000;

struct Parsed_text
{
    char **arr = nullptr;
    size_t size = 0;
    size_t capacity = 0;
};

/// public
Parsed_text *parce_text (const char *filename);
void parced_text_dtor (Parsed_text *parced_text);

/// private 
void parced_arr_dtor (char **arr, size_t size);
int realloc_text (Parsed_text *parced_text);
bool is_contains (char **word_arr, const char *word, const size_t size);

char *read_text (const char *filename);
size_t get_file_size (const char *filename);

#endif /* PARSER_H */