#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <dlfcn.h>

#include "threads_args.h"
#include "utils.h"

#define MAX_THREADS 10

thread_arg_t *threads = NULL;
process_fn process_chunk = NULL;
pthread_mutex_t mutex;
long total_count = 0;

long parse_limit(const char *str)
{
    errno = 0;
    char *endptr = NULL;
    long limit = strtol(str, &endptr, 10);

    if (errno == ERANGE || *endptr != '\0' || endptr == str)
    {
        fprintf(stderr, "Ошибка: '%s' не является корректным числом\n", str);
        exit(EXIT_FAILURE);
    }
    if (limit < 0)
    {
        fprintf(stderr, "Ошибка: лимит не может быть отрицательным\n");
        exit(EXIT_FAILURE);
    }
    return limit;
}

void build_output_name(char *buf, size_t size, int index)
{
    snprintf(buf, size, "output%d.txt", index);
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        fprintf(stderr, "Использование: %s <файл1> [файл2 ...] <лимит>\n", argv[0]);
        return -1;
    }

    long limit = parse_limit(argv[argc - 1]);

    int n_files = argc - 2;

    if (n_files > MAX_THREADS)
    {
        fprintf(stderr, "Ошибка: максимум %d файлов, передано %d\n",
                MAX_THREADS, n_files);
        return -1;
    }

    threads = malloc(n_files * sizeof(thread_arg_t));
    if (threads == NULL)
    {
        perror("malloc threads");
        return -1;
    }

    int rc = pthread_mutex_init(&mutex, NULL);
    if (rc != 0)
    {
        fprintf(stderr, "Ошибка pthread_mutex_init: %s\n", strerror(rc));
        free(threads);
        return -1;
    }

    void *handle = dlopen("./lib/libprocess.so", RTLD_LAZY);
    if (handle == NULL)
    {
        fprintf(stderr, "Ошибка dlopen: %s\n", dlerror());
        pthread_mutex_destroy(&mutex);
        free(threads);
        return -1;
    }

    process_chunk = (process_fn)dlsym(handle, "process_chunk");
    if (process_chunk == NULL)
    {
        fprintf(stderr, "Ошибка dlsym: %s\n", dlerror());
        dlclose(handle);
        pthread_mutex_destroy(&mutex);
        free(threads);
        return -1;
    }

    pthread_t tids[MAX_THREADS];

    for (int i = 0; i < n_files; i++)
    {
        threads[i].input  = argv[i + 1];
        threads[i].limit  = limit;
        threads[i].count  = 0;
        threads[i].status = 0;

        build_output_name(threads[i].output, sizeof(threads[i].output), i + 1);

        rc = pthread_create(&tids[i], NULL, worker, &threads[i]);
        if (rc != 0)
        {
            fprintf(stderr, "Ошибка pthread_create для потока %d: %s\n",
                    i + 1, strerror(rc));
            for (int j = 0; j < i; j++)
                pthread_join(tids[j], NULL);
            dlclose(handle);
            pthread_mutex_destroy(&mutex);
            free(threads);
            return -1;
        }
    }

    for (int i = 0; i < n_files; i++)
    {
        pthread_join(tids[i], NULL);
        printf("Поток %d: %s → %ld вставок (status %d)\n",
               i + 1, threads[i].input, threads[i].count, threads[i].status);
    }

    printf("Всего вставок: %ld\n", total_count);

    dlclose(handle);
    pthread_mutex_destroy(&mutex);
    free(threads);

    return 0;
}