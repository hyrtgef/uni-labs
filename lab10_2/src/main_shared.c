#include <stdio.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#include "utils.h"

typedef int (*process_fn)(const char *in, size_t in_len,
                          char *out, size_t out_cap,
                          size_t *out_len, long limit,
                          long *count);

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        fprintf(stderr, "Использование: %s input.txt output.txt N\n", argv[0]);
        return -1;
    }

    errno = 0;
    char *endptr = NULL;
    long limit = strtol(argv[3], &endptr, 10);
    if (errno == ERANGE || *endptr != '\0' || endptr == argv[3])
    {
        fprintf(stderr, "Ошибка: не является корректным числом\n");
        return -1;
    }
    if (limit < 0)
    {
        fprintf(stderr, "Ошибка: количетсво вставок не может быть отрицательным\n");
        return -1;
    }

    void *library_handler;
    library_handler = dlopen("./libprocess.so", RTLD_LAZY);
    if (!library_handler)
    {
        fprintf(stderr, "Ошибка dlopen: %s\n", dlerror());
        return -1;
    }

    process_fn process_chunk = NULL;
    *(void **)(&process_chunk) = dlsym(library_handler, "process_chunk");
    if(!process_chunk)
    {
        fprintf(stderr, "Ошибка dlsym: %s\n", dlerror());
        dlclose(library_handler);
        return -1;
    }

    int fd_in = Open(argv[1], O_RDONLY);
    if(fd_in == -1)
    {
        dlclose(library_handler);
        return -1;   
    }

    if(LockRead(fd_in) == -1)
    {
        Close(fd_in);
        dlclose(library_handler);
        return -1;
    }

    int fd_out = Open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd_out == -1)
    {
        Close(fd_in);
        dlclose(library_handler);
        return -1;
    }

    if(LockWrite(fd_out) == -1)
    {
        Close(fd_in);
        Close(fd_out);
        dlclose(library_handler);
        return -1;
    }

    char in[BUF_SIZE], out[BUF_SIZE];
    size_t out_len = 0;
    long count = 0;
    ssize_t n;
    
    while((n = Read(fd_in, in, BUF_SIZE)) > 0)
    {
        out_len = 0;
        if(process_chunk(in, (size_t)n, out, BUF_SIZE, &out_len, limit, &count) == -1)
        {
            fprintf(stderr, "Ошибка: выходной буфер переполнен\n");
            Close(fd_in);
            Close(fd_out);
            dlclose(library_handler);
            return -1;
        }

        if(out_len > 0)
        {
            if(Write(fd_out, out, out_len) == -1)
            {
                Close(fd_in);
                Close(fd_out);
                dlclose(library_handler);
                return -1;
            }
        }
    }

    if(n == -1)
    {
        Close(fd_in);
        Close(fd_out);
        dlclose(library_handler);
        return -1;
    }

    Close(fd_in);
    Close(fd_out);

    dlclose(library_handler);

    printf("Вставлено точек: %ld\n", count);
    return 0;
}