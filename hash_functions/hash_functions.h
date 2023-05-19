#ifndef HASH_FUNCTIONS_H
#define HASH_FUNCTIONS_H

#include <stdio.h>
#include <assert.h>
#include <string.h>

size_t hash_ret_1 (const char *string);
size_t hash_first_ch (const char *string);
size_t hash_strlen (const char *string);
size_t hash_ch_sum (const char *string);
size_t hash_rotr (const char *string);
size_t hash_rotl (const char *string);
size_t hash_crc64 (const char *string);
size_t hash_crc64_opt (const char string[32]);
size_t hash_crc64_char32 (const char string[32]);

size_t rotate_right (size_t val);
size_t rotate_left (size_t val);

#endif /* HASH_FUNCTIONS_H */