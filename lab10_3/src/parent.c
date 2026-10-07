#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#define APP_PATH "./app_shared"

long parse_limit(const char *str)
{
    errno = 0;
    char *endptr = NULL;
    long limit = strtol(str, &endptr, 10);

    if (errno == ERANGE || *endptr != '\0' || endptr == str)
    {
        fprintf(stderr, "Ошибка: не является корректным числом\n");
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

void exec_child(const char *input, const char *output, const char *limit_str)
{
    execl(APP_PATH, APP_PATH, input, output, limit_str, NULL);

    perror("execl");
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        fprintf(stderr, "Использование: %s input1.txt [input2.txt ...] N\n", argv[0]);
        return -1;
    }

    long limit = parse_limit(argv[argc - 1]);

    int n_files = argc - 2;

    pid_t *pids = malloc(n_files * sizeof(pid_t));
    if (pids == NULL)
    {
        perror("malloc");
        return -1;
    }

    for (int i = 0; i < n_files; i++)
    {
        const char *input = argv[i + 1];
        char output[64];
        build_output_name(output, sizeof(output), i + 1);

        pid_t pid = fork();
        if (pid == -1)
        {
            perror("fork");
            free(pids);
            return -1;
        }
        if (pid == 0)
        {
            exec_child(input, output, argv[argc - 1]);
        }

        pids[i] = pid;
    }

    for (int i = 0; i < n_files; i++)
    {
        int status;
        if (waitpid(pids[i], &status, 0) == -1)
        {
            perror("waitpid");
            continue;
        }
        if (WIFEXITED(status))
        {
            printf("Процесс %d (файл %s) завершен с кодом %d\n", pids[i], argv[i + 1], WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            printf("Процесс %d убит сигналом %d\n", pids[i], WTERMSIG(status));
        }
    }
    free(pids);
    return 0;
}