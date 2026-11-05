#include <pthread.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "threads_args.h"
#include "utils.h"

void *worker(void *arg)
{
    thread_arg_t *t = (thread_arg_t *)arg;

    int fd_in = Open(t->input, O_RDONLY);
    char buf[BUF_SIZE];
    ssize_t n = Read(fd_in, buf, BUF_SIZE);
    Close(fd_in);

    char out[BUF_SIZE];
    size_t out_len = 0;

    if(process_chunk(buf, (size_t)n, out, BUF_SIZE, &out_len, t->limit, &t->count) == -1)
    {
        t->status = -1;
        t->count = -1;
        return NULL;
    }

    int fd_out = Open(t->output, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    Write(fd_out, out, out_len);
    Close(fd_out);

    pthread_mutex_lock(&mutex);
    total_count += t->count;
    printf("Поток обработал %s: %ld вставок\n", t->input, t->count);
    pthread_mutex_unlock(&mutex);

    t->status = 0;
    return NULL;
}
