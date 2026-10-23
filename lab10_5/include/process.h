#ifndef PROCESS_H
#define PROCESS_H

#include <stddef.h>

int process_chunk(const char *in, size_t in_len, char* out,
                        size_t out_cap, size_t* out_len,
                        long limit, long* count);

#endif