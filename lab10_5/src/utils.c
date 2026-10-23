#include <stdio.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <stddef.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mqueue.h>

#include "utils.h"

int Open(const char *pathname, int flags, ...)
{
    if (pathname == NULL)
    {
        fprintf(stderr, "Ошибка открытия файла\n");
        exit(EXIT_FAILURE);
    }

    mode_t mode = 0;
    int fd = 0;

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
    }

    return fd;
}

ssize_t Write(int fd, const void *buf, size_t count)
{
    if (buf == NULL)
    {
        fprintf(stderr, "Ошибка: буфер пуст\n");
        exit(EXIT_FAILURE);
    }

    ssize_t n = write(fd, buf, count);

    if (n == -1)
    {
        fprintf(stderr, "Ошибка записи в файл fd = %d: %s\n", fd, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return n;
}

ssize_t Read(int fd, void *buf, size_t count)
{
    if (buf == NULL)
    {
        fprintf(stderr, "Ошибка буффер пуст\n");
        exit(EXIT_FAILURE);
    }

    ssize_t n = read(fd, buf, count);

    if (n == -1)
    {
        fprintf(stderr, "Ошибка чтение файла fd = %d : %s\n", fd, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return n;
}

int Close(int fd)
{
    if (close(fd) == -1)
    {
        fprintf(stderr, "Ошибка закрытия файла fd = %d\n", fd);
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
        fprintf(stderr, "Ошибка блокировки на запись %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return 0;
}

mqd_t Mq_open(const char *name, int oflag, mode_t mode, struct mq_attr *attr)
{
    mqd_t md = mq_open(name, oflag, mode, attr);

    if (md == (mqd_t)-1)
    {
        fprintf(stderr, "Ошибка: mq_open '%s': %s\n", name, strerror(errno));
        exit(EXIT_FAILURE);
    }

    return md;
}

int Mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned int msg_prio)
{
    int mqs = mq_send(mqdes, msg_ptr, msg_len, msg_prio);

    if (mqs == -1)
    {
        fprintf(stderr, "Ошибка mq_send: %s \n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return mqs;
}

ssize_t Mq_receive(mqd_t mqdes, void *msg_ptr, size_t msg_len, unsigned int *msg_prio)
{
    ssize_t mqr = mq_receive(mqdes, msg_ptr, msg_len, msg_prio);
    if(mqr == -1)
    {
        fprintf(stderr, "Ошибка mq_receive: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return mqr;
}

int Mq_close(mqd_t mqdes)
{
    if(mq_close(mqdes) == (mqd_t)-1)
    {
        fprintf(stderr, "Ошибка mq_close: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return 0;
}

int Mq_unlink(const char *name)
{
    if(mq_unlink(name) == -1)
    {
        fprintf(stderr, "Ошибка mq_unlink: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return 0;
}