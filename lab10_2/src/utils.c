#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "utils.h"

int Open(const char *pathname, int flags, ...)
{
    if (pathname == NULL)
    {
        fprintf(stderr, "Ошибка: Open: pathname == NULL\n");
        return -1;
    }

    mode_t mode = 0;
    int fd;

    if (flags & O_CREAT)
    {
        va_list ap;
        va_start(ap, flags);
        mode = (mode_t)va_arg(ap, int);
        va_end(ap);
    }

    fd = open(pathname, flags, mode);
    if (fd == -1)
    {
        fprintf(stderr, "Ошибка: открытия файла '%s': %s\n", pathname, strerror(errno));
        return -1;
    }
    return fd;
}

ssize_t Read(int fd, void *buf, size_t count)
{
    ssize_t n;
    if (buf == NULL)
    {
        fprintf(stderr, "Ошибка: Read: buf == NULL\n");
        return -1;
    }

    n = read(fd, buf, count);
    if (n == -1)
    {
        fprintf(stderr, "Ошибка чтения файла fd =%d: %s\n", fd, strerror(errno));
        return -1;
    }
    return n;
}

ssize_t Write(int fd, const void *buf, size_t count)
{
    ssize_t n;
    if (buf == NULL)
    {
        fprintf(stderr, "Ошибка: Write: buf == NULL\n");
        return -1;
    }

    n = write(fd, buf, count);
    if (n == -1)
    {
        fprintf(stderr, "Ошибка записи в файл fd = %d: %s\n", fd, strerror(errno));
        return -1;
    }

    return n;
}

int Close(int fd)
{
    if (close(fd) == -1)
    {
        fprintf(stderr, "Ошибка закрытия файла %d: %s\n", fd, strerror(errno));
        return -1;
    }
    return 0;
}

void print_error(const char *msg)
{
    if (msg == NULL)
    {
        msg = "Неизвестная ошибка";
    }
    fprintf(stderr, "Ошибка %s : %s\n", msg, strerror(errno));
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
        fprintf(stderr, "Ошибка, не удалось заблокировать файл на чтение: %s\n", strerror(errno));
        return -1;
    }
    return  0;
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
        fprintf(stderr, "Ошибка, не удалось заблокировать процесс на запись: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}