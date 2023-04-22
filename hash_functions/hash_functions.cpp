#include "hash_functions.h"

size_t hash_1 (char *string)
{
    assert (string);
    
    return 1;
}

size_t hash_2 (char *string)
{
    assert (string);
    
    return *string;
}

size_t hash_3 (char *string)
{
    assert (string);
    
    return strlen (string);
}

size_t hash_4 (char *string)
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

size_t hash_5 (char *string)
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

size_t hash_6 (char *string)
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
