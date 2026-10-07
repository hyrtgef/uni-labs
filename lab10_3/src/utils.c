#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

int Open(const char *pathname, int flag, ...)
{
    if (pathname == NULL)
    {
        fprintf(stderr, "Ошибка открытия файла\n");
        exit(1);
    }

    mode_t mode = 0;
    int fd;

    if (flag & O_CREAT)
    {
        va_list ap;
        va_start(ap, flag);
        mode = (mode_t)va_arg(ap, int);
        va_end(ap);
    }

    fd = open(pathname, flag, mode);
    if (fd == -1)
    {
        fprintf(stderr, "Ошибка: открытия файла '%s': %s\n", pathname, strerror(errno));
        exit(1);
    }
    return fd;
}

ssize_t Read(int fd, void *buf, size_t count)
{
    ssize_t n;
    if (buf == NULL)
    {
        fprintf(stderr, "Ошибка буфер пуст\n");
        exit(1);
    }

    n = read(fd, buf, count);
    if (n == -1)
    {
        fprintf(stderr, "Ошибка чтение файла fd=%d: %s", fd, strerror(errno));
        exit(1);
    }
    return n;
}

ssize_t Write(int fd, const void *buf, size_t count)
{
    ssize_t n;
    if (buf == NULL)
    {
        fprintf(stderr, "Ошибка: буфер пуст\n");
        exit(1);
    }

    n = write(fd, buf, count);
    if (n == -1)
    {
        fprintf(stderr, "Ошибка записи в файл fd= %d: %s\n", fd, strerror(errno));
        exit(1);
    }
    return n;
}

int Close(int fd)
{
    if(close(fd) == -1)
    {
        fprintf(stderr, "Ошибка закрытия файла fd =%d\n", fd);
        exit(1);
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
    if(fcntl(fd , F_SETLK, &fl) == -1)
    {
        fprintf(stderr, "Ошибка блокировки на чтение %s\n", strerror(errno));
        exit(1);
    } 
    return 0;
}

int LockWrite(int fd)
{
    struct flock fl ={0};
    fl.l_type =F_WRLCK;
    fl.l_whence =SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;
    if(fcntl(fd, F_SETLK, &fl) == -1)
    {
        fprintf(stderr, "Ошибка блокировки на запись %s\n", strerror(errno));
        exit(1);
    }
    return 0;
}
