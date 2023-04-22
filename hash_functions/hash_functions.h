#ifndef HASH_FUNCTIONS_H
#define HASH_FUNCTIONS_H

#include <stdio.h>
#include <assert.h>
#include <string.h>

size_t hash_1 (char *string);
size_t hash_2 (char *string);
size_t hash_3 (char *string);
size_t hash_4 (char *string);
size_t hash_5 (char *string);
size_t hash_6 (char *string);

size_t rotate_right (size_t val);
size_t rotate_left (size_t val);

#endif /* HASH_FUNCTIONS_H */