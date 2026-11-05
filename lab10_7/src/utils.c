#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "utils.h"

int Open(const char *pathname, int flags, ...)
{
    if (pathname == NULL)
    {
        fprintf(stderr, "Ошибка имя не может быть NULL\n");
        exit(EXIT_FAILURE);
    }

    int fd = 0;
    mode_t mode = 0;

    if (flags & O_CREAT)
    {
        va_list pa;
        va_start(pa, flags);
        mode = (mode_t)va_arg(pa, int);
        va_end(pa);
    }

    fd = open(pathname, flags, mode);
    if (fd == -1)
    {
        fprintf(stderr, "Ошибка открытия файла '%s': %s\n", pathname, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return fd;
}

ssize_t Write(int fd, const void *buf, size_t count)
{
    if (buf == NULL)
    {
        fprintf(stderr, "Ошибка пустой буффер\n");
        exit(EXIT_FAILURE);
    }

    ssize_t n = write(fd, buf, count);
    if (n == -1)
    {
        fprintf(stderr, "Ошибка записи файла fd = %d: %s\n", fd, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return n;
}

ssize_t Read(int fd, void *buf, size_t count)
{
    if (buf == NULL)
    {
        fprintf(stderr, "Ошибка пустой буффер\n");
        exit(EXIT_FAILURE);
    }

    ssize_t n = read(fd, buf, count);
    if (n == -1)
    {
        fprintf(stderr, "Ошибка чтения файла fd = %d: %s\n", fd, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return n;
}

int Close(int fd)
{
    if (close(fd) == -1)
    {
        fprintf(stderr, "Ошибка закрытия файла\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}

int LockRead(int fd)
{
    struct flock fl = {0};
    fl.l_type = F_RDLCK;      
    fl.l_whence = SEEK_SET;   
    fl.l_start = 0;
    fl.l_len = 0;
    if (fcntl(fd, F_SETLK, &fl) == -1)
    {
        fprintf(stderr, "Ошибка блокировки на чтение: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return 0;
}

int LockWrite(int fd)
{
    struct flock fl = {0};
    fl.l_type = F_WRLCK;     
    fl.l_whence = SEEK_SET;    
    fl.l_start = 0;
    fl.l_len = 0;
    if (fcntl(fd, F_SETLK, &fl) == -1)
    {
        fprintf(stderr, "Ошибка блокировки на запись: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return 0;
}
