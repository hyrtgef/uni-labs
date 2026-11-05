#ifndef THREADS_ARGS_H
#define THREADS_ARGS_H

#include <stddef.h>
#include <pthread.h>

typedef int (*process_fn)(const char *, size_t, char*,
                         size_t ,size_t* ,
                        long, long*);

typedef struct
{
    const char *input;
    char output[64];
    long limit; 
    long count;
    int status;
}thread_arg_t;

extern thread_arg_t *threads;
extern process_fn process_chunk;
extern pthread_mutex_t mutex;
extern long total_count;

void *worker(void *arg);



#endif