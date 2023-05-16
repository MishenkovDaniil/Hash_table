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
            size_t first_bit = crc >> (sizeof (unsigned int) * 8 - 1);
            size_t first_ch_bit = cur_ch >> (sizeof (char) * 8 - 1);
            crc = (crc << 1) + first_ch_bit;
            cur_ch <<= 1;

            if (first_bit)
            {
                crc = inverse_crc (crc, CRC_64_MASK);
            }
        }
        if (!temp)
            break;
        cur_ch = *string++;
    }

    return crc;
}

size_t hash_crc64_opt (char *string)
{
    size_t size = strlen (string);
    size_t shift = size % sizeof (size_t);
    size_t result = 0;

    size = size - shift  + sizeof (size_t);

    for (size_t iter = 0; iter < size / 8; ++iter)
    {
        size_t buf = 0;
        memcpy (&buf, string, iter + 1 < size / 8 ? sizeof (size_t) : shift);

        result = _mm_crc32_u64 (result, buf);
        string += sizeof (size_t);
    }

    return result;
}

size_t inverse_crc (size_t crc, const size_t crc_mask)
{
    return crc ^ crc_mask;
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

