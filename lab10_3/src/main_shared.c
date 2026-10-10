#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <errno.h>
#include <dlfcn.h>

#include "utils.h"

typedef int (*process_fn)(const char *, size_t, char *,
                          size_t, size_t *, long, long *);

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        fprintf(stderr, "Использование: %s input.txt output.txt N\n", argv[0]);
        return -1;
    }

    errno = 0;
    char *endptr = NULL;
    long limit = strtol(argv[argc - 1], &endptr, 10);
    if (errno == ERANGE || *endptr != '\0' || endptr == argv[argc - 1])
    {
        fprintf(stderr, "Ошибка: неверный ввод\n");
        return -1;
    }
    if (limit < 0)
    {
        fprintf(stderr, "Ошибка лимит не может быть отрицательным\n");
        return -1;
    }

    int fd_in = Open(argv[1], O_RDONLY);
    if (fd_in == -1)
    {
        return -1;
    }

    if (LockRead(fd_in) == -1)
    {
        Close(fd_in);
        return -1;
    }

    int fd_out = Open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_out == -1)
    {
        Close(fd_in);
        return -1;
    }

    if (LockWrite(fd_out) == -1)
    {
        Close(fd_in);
        Close(fd_out);
        return -1;
    }

    void *handle = dlopen("./libprocess.so", RTLD_LAZY);
    if (handle == NULL)
    {
        fprintf(stderr, "dlopen: %s\n", dlerror());
        Close(fd_in);
        Close(fd_out);
        return -1;
    }

    process_fn process_chunk = (process_fn)dlsym(handle, "process_chunk");
    if (process_chunk == NULL)
    {
        fprintf(stderr, "dlsym: %s\n", dlerror());
        dlclose(handle);
        Close(fd_in);
        Close(fd_out);
        return -1;
    }

    char in[BUF_SIZE], out[BUF_SIZE];
    size_t out_len = 0;
    long count = 0;
    ssize_t n;

    while ((n = Read(fd_in, in, BUF_SIZE)) > 0)
    {
        out_len = 0;
        if (process_chunk(in, (size_t)n, out, BUF_SIZE, &out_len, limit, &count) == -1)
        {
            fprintf(stderr, "Переполнение буфера\n");
            dlclose(handle);
            Close(fd_in);
            Close(fd_out);
            return -1;
        }

        if (out_len > 0)
        {
            if (Write(fd_out, out, out_len) == -1)
            {
                dlclose(handle);
                Close(fd_in);
                Close(fd_out);
                return -1;
            }
        }
    }

    if (n == -1)
    {
        dlclose(handle);
        Close(fd_in);
        Close(fd_out);
        return -1;
    }

    dlclose(handle);
    Close(fd_in);
    Close(fd_out);

    printf("Вставлено точек %ld\n", count);

    return 0;
}
