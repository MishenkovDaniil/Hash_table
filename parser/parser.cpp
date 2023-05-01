
#include "parser.h"

Parsed_text *parce_text (const char *filename)
{
    assert (filename);

    char *text = read_text (filename);
    assert (text);

    Parsed_text *parsed_text = (Parsed_text *)calloc (1, sizeof (Parsed_text));

    parsed_text->arr = (char **)calloc (START_SIZE, sizeof (char *));
    assert (parsed_text->arr);
    parsed_text->capacity = START_SIZE;
    
    int idx = 0;

    char *cur_smbl_ptr = text;

    char *buf = (char *)calloc (MAX_STR_SIZE, sizeof(char));
    
    while (*(cur_smbl_ptr))
    {
        char cur_smbl = *cur_smbl_ptr;
        size_t word_size = 0;

        if (isspace (cur_smbl) || ispunct (cur_smbl) || isdigit (cur_smbl))
        {
            ++cur_smbl_ptr;
            continue;
        }

        sscanf (cur_smbl_ptr, "%[a-zA-z_1-9]%n", buf, &word_size);
        buf[word_size] = '\0';
        
        if (!word_size)
        {
            ++cur_smbl_ptr;
            continue;
        }
        
        // if (is_contains (parsed_text->arr, buf, idx))
        // {
        //     cur_smbl_ptr += word_size;
        //     continue;
        // }

        parsed_text->arr[idx] = (char *)calloc (word_size + 1, sizeof (char));
        sprintf (parsed_text->arr[idx], "%s", buf);
        fprintf (stderr, "[%s]\n", parsed_text->arr[idx]);

        cur_smbl_ptr += word_size;
        ++idx;

        if (idx == parsed_text->capacity)
        {
            realloc_text (parsed_text); //check on -1
        }
    }

    parsed_text->size = idx;

    free (buf);
    free (text);
    
    return parsed_text;
}

// bool is_contains (char **word_arr, const char *word, const size_t size)
// {
//     assert (word_arr && word);

//     for (size_t idx = 0; idx < size; ++idx)
//     {
//         if (!(strcasecmp (word_arr[idx], word)))
//         {
//             return true;
//         }
//     }

//     return false;
// }

int realloc_text (Parsed_text *parsed_text)
{
    assert (parsed_text && parsed_text->arr);

    parsed_text->arr = (char **)realloc ((void *)parsed_text->arr, sizeof(char *) * parsed_text->capacity * 2);
    assert (parsed_text->arr);  ///return -1

    parsed_text->capacity *= 2;

    return 0;
}

void parced_text_dtor (Parsed_text *parsed_text)
{
    if (!(parsed_text && parsed_text->arr))
        return;
    
    parced_arr_dtor (parsed_text->arr, parsed_text->size);

    free (parsed_text->arr);
    parsed_text->arr = nullptr;

    free (parsed_text);
    parsed_text = nullptr;
}

void parced_arr_dtor (char **arr, size_t size)
{
    int idx = 0;
    while (idx < size)
    {
        if (arr[idx])
        {
            free (arr[idx]);
            arr[idx] = nullptr;
        }

        ++idx;
    }
}


char *read_text (const char *filename)
{
    assert (filename);

    int file_descr = open (filename, O_RDONLY);
    size_t filesize = get_file_size (filename);

    char *buf = (char *)calloc (filesize + 1, sizeof (char));
    assert (buf);

    int status = read (file_descr, buf, filesize);
    
    if (status == -1)
    {
        perror ("read () error: ");
        return nullptr;
    }

    printf ("read %d from %lu\n", status, filesize);

    return buf;
}

size_t get_file_size (const char *filename)
{
    struct stat buf = {};

    if (stat (filename, &buf) == -1)
    {
        perror ("stat() failed.\n");

        return 0;
    }

    return buf.st_size;
}