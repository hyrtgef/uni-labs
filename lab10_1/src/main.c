#include <stdio.h>
#include <fcntl.h>
#include <limits.h>
#include <errno.h>
#include <stdlib.h>

#include "utils.h"

int main(int argc, char *argv[])
{
    /*проверка аргументов*/
    if (argc != 4)
    {
        fprintf(stderr, "Ошибка:%s ./app input.txt output.txt N\n", argv[0]);
        return -1;
    }

    /*парсинг лимита*/
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
        fprintf(stderr, "Ошибка: колличество вставок не может быть отрицательным\n");
        return -1;
    }

    /*открытие входного файла*/
    int fd_in = Open(argv[1], O_RDONLY);
    if (fd_in == -1)
    {
        return -1;
    }

    /*shared lock*/
    if (LockRead(fd_in) == -1)
    {
        Close(fd_in);
        return -1;
    }

    /*открытие выходного файла*/
    int fd_out = Open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_out == -1)
    {
        Close(fd_in);
        return -1;
    }

    /*exclusive lock*/
    if(LockWrite(fd_out) == -1)
    {
        Close(fd_in);
        Close(fd_out);
        return -1;
    }



    char in[BUF_SIZE], out[BUF_SIZE]; // входной выходной буфер
    size_t out_len = 0;               // счетчик заполнения
    long count = 0;                   // счетчик вставок
    ssize_t n;                        // колво посчитаных байтов

    /*главный цикл*/
    while ((n = Read(fd_in, in, BUF_SIZE)) > 0)
    {
        for (size_t i = 0; i < (size_t)n; i++)
        {
            if (out_len + 2 > BUF_SIZE)
            {
                if (Write(fd_out, out, out_len) == -1)
                {
                    fprintf(stderr, "Ошибка: запись в выходной файл\n");
                    Close(fd_in);
                    Close(fd_out);
                    return -1;
                }
                out_len = 0;
            }

            char c = in[i];

            if (c == ' ' && count < limit)
            {
                out[out_len++] = ' ';
                out[out_len++] = '.';
                count++;
            }
            else
            {
                out[out_len++] = c;
            }
        }
    }

    /*проверка ошибок чтения*/
    if (n == -1)
    {
        fprintf(stderr, "Ошибка: чтение входного файла\n");
        Close(fd_in);
        Close(fd_out);
        return -1;
    }

    /*сброс отстатка*/
    if (out_len > 0)
    {
        if (Write(fd_out, out, out_len) == -1)
        {
            fprintf(stderr, "Ошибка: запись отстатка\n");
            Close(fd_in);
            Close(fd_out);
            return -1;
        }
    }

    Close(fd_in);
    Close(fd_out);

    printf("Вставлено точек %ld\n", count);
    return 0;
}