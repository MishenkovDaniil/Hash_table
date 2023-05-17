#ifndef HASH_FUNCTIONS_H
#define HASH_FUNCTIONS_H

#include <stdio.h>
#include <assert.h>
#include <string.h>

size_t hash_ret_1 (char *string);
size_t hash_first_ch (char *string);
size_t hash_strlen (char *string);
size_t hash_ch_sum (char *string);
size_t hash_rotr (char *string);
size_t hash_rotl (char *string);
size_t hash_crc64 (char *string);
size_t hash_crc64_opt (char string[32]);
size_t hash_crc64_char32 (char string[32]);

size_t rotate_right (size_t val);
size_t rotate_left (size_t val);

#endif /* HASH_FUNCTIONS_H */