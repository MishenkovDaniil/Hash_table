#include <nmmintrin.h>

#include "hash_functions.h"

static const size_t CRC_64_MASK = 0x42F0E1EBA9EA3693; 

size_t hash_ret_1 (char *string)
{
    assert (string);
    
    return 1;
}

size_t hash_first_ch (char *string)
{
    assert (string);
    
    return *string;
}

size_t hash_strlen (char *string)
{
    assert (string);
    
    return strlen (string);
}

size_t hash_ch_sum (char *string)
{
    assert (string);
    
    size_t sum = 0;

    while (*string)
    {
        sum += (size_t)*string;
        ++string;
    }

    return sum;
}

size_t hash_rotr (char *string)
{
    assert (string);
    
    size_t sum = 0;

    while (*string)
    {
        sum = rotate_right (sum);
        sum = sum^(size_t)*string;
        
        ++string;
    }

    return sum;
}

size_t hash_rotl (char *string)
{
    assert (string);
    
    size_t sum = 0;

    while (*string)
    {
        sum = rotate_left (sum);
        sum = sum^(size_t)*string;
        
        ++string;
    }

    return sum;
}

size_t hash_crc64 (char *string)
{
    size_t crc = 0;

    char cur_ch = *string++;

    while (1)
    {
        char temp = cur_ch;

        for (size_t i = 0; i < sizeof (char) * 8; ++i)
        {
            size_t first_bit = crc >> (sizeof (size_t) * 8 - 1);
            size_t first_ch_bit = cur_ch >> (sizeof (char) * 8 - 1);

            crc = (crc << 1) + first_ch_bit;
            cur_ch <<= 1;

            if (first_bit)
            {
                crc = crc ^ CRC_64_MASK;
            }
        }
        if (!temp)
            break;
        cur_ch = *string++;
    }

    return crc;
}

size_t hash_crc64_char32 (char string[32])
{
    const size_t LEN = 32;
    const size_t BIT_NUM = 8;
    size_t crc = 0;

    for (int idx = 0; idx < LEN; ++idx)
    {
        char cur_ch = string[idx];

        for (size_t i = 0; i < sizeof (char) * BIT_NUM; ++i)
        {
            size_t first_bit = crc >> (sizeof (size_t) * BIT_NUM - 1);
            size_t first_ch_bit = cur_ch >> (sizeof (char) * BIT_NUM - 1);

            crc = (crc << 1) + first_ch_bit;
            cur_ch <<= 1;

            if (first_bit)
            {
                crc = crc ^ CRC_64_MASK;
            }
        }
    }

    return crc;
}

size_t hash_crc64_opt (char string[32])
{
    const size_t LEN = 32;
    const size_t ITER_NUM = LEN / sizeof (size_t);
    
    size_t result = 0;

    for (size_t iter = 0; iter < ITER_NUM; ++iter)
    {
        result = _mm_crc32_u64 (result, (size_t)string);
        string += sizeof (size_t);
    }

    return result;
}

size_t rotate_right (size_t val)
{
    size_t first_bit = val << (sizeof (val) * 8 - 1);
    size_t rotated_val = first_bit | (val >> 1);

    return rotated_val;
}

size_t rotate_left (size_t val)
{
    size_t first_bit = val >> (sizeof (val) * 8 - 1);
    size_t rotated_val = first_bit | (val << 1);

    return rotated_val;
}

