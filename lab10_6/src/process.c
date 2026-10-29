#include "process.h"

int process_chunk(const char *in, size_t in_len,
                   char *out, size_t out_cap, size_t *out_len,
                   long limit, long *count)
{
    size_t olen = 0;

    for (size_t i = 0; i < in_len; i++)
    {
        if (olen + 2 > out_cap)
        {
            *out_len = olen;
            return -1;
        }

        char c = in[i];

        if (c == ' ' && *count < limit)
        {
            out[olen++] = ' ';
            out[olen++] = '.';
            (*count)++;
        }else
        {
            out[olen++] = c;
        }
    }
    *out_len = olen;
    return 0;
}