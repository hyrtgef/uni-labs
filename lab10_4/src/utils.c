#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
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
    int fd = 0;

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
        fprintf(stderr, "Ошибка открытия файла '%S' : '%S'", pathname, strerror(errno));
        exit(1);
    }
    return fd;
}

ssize_t Write(int fd, const void *buf, size_t count)
{
    ssize_t n;

    if (buf == NULL)
    {
        fprintf(stderr, "Ошибка буффер пуст\n");
        exit(1);
    }

    n = write(fd, buf, count);
    if (n == -1)
    {
        fprintf(stderr, "Ошибка записи в файл fd =%d: %s\n", fd, strerror(errno));
        exit(1);
    }
    return n;
}

ssize_t Read(int fd, const void *buf, size_t count)
{
    ssize_t n;

    if (buf == NULL)
    {
        fprintf(stderr, "Ошибка буффер пуст \n");
        exit(1);
    }

    n = read(fd, buf, count);
    if (n == -1)
    {
        fprintf(stderr, "Ошибка чтения файла fd = %d: %s\n", fd, strerror(errno));
        exit(1);
    }
    return n;
}

int Close(int fd)
{
    if (close(fd) == -1)
    {
        fprintf(stderr, "Ошибка закрытия файла fd = %d", fd);
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
    if (fcntl(fd, F_SETLK, &fl) == -1)
    {
        fprintf(stderr, "Ошибка блокировки на чтение: %s\n", strerror(errno));
        exit(1);
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
        fprintf(stderr, "Ошибка блокировки записи файла: %s", strerror(errno));
        exit(1);
    }
    return 0;
}

int Mkfifo(const char *pathname, mode_t mode)
{
    if (pathname == NULL)
    {
        fprintf(stderr, "Ошибка имя канала не может быть NULL\n");
        exit(1);
    }

    if (mkfifo(pathname, mode) == -1)
    {
        if (errno == EEXIST)
        {
            return 0;
        }

        fprintf(stderr, "Ошибка: mkfifo не удалось создать '%s': %s\n", pathname, strerror(errno));
        exit(1);
    }
    return 0;
}

int Unlink(const char *pathname)
{
    if(pathname == NULL)
    {
        fprintf(stderr, "Ошибка имя файла не может быть NULL\n");
        exit(1);
    }    

    if(unlink(pathname) == -1)
    {
        if(errno == ENOENT)
        {
            fprintf(stderr, "Предупреждение файла '%s' уже не существует\n", pathname);
            return 0;
        }
        fprintf(stderr, "Ошибка удления файла '%s': %s\n", pathname, strerror(errno));
        exit(1);
    }
    return 0;
}