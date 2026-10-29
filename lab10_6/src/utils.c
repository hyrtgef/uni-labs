#define _POSIX_C_SOURCE 200809L

#include "utils.h"

#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <stdint.h>

int Open(const char *pathname, int flags, ...)
{
    if (pathname == NULL)
    {
        fprintf(stderr, "Ошибка открытия файла\n");
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
        fprintf(stderr, "Ошибка буффер пуст\n");
        exit(EXIT_FAILURE);
    }

    ssize_t n = write(fd, buf, count);
    if (n == -1)
    {
        fprintf(stderr, "Ошибка записи в файл fd =%d: %s\n", fd, strerror(errno));
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
        fprintf(stderr, "Ошибка чтения файла fd = %d: %s\n", fd, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return n;
}

int Close(int fd)
{
    if (close(fd) == -1)
    {
        fprintf(stderr, "Ошибка закрытия файла fd = %d: %s\n", fd, strerror(errno));
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
        fprintf(stderr, "Ошибка блокировки на чтение %s\n", strerror(errno));
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

sem_t *Sem_open(const char *name, int oflag, mode_t mode, unsigned int value)
{
    if (name == NULL)
    {
        fprintf(stderr, "Ошибка семафор не существует\n");
        exit(EXIT_FAILURE);
    }

    sem_t *sem = sem_open(name, oflag, mode, value);

    if (sem == SEM_FAILED)
    {
        if ((oflag & O_EXCL) && errno == EEXIST)
        {
            fprintf(stderr, "Ошибка sem_open: Семафор '%s' уже существует\n", name);
        }
        else
        {
            fprintf(stderr, "Ошибка sem_open для '%s': %s\n", name, strerror(errno));
        }
        exit(EXIT_FAILURE);
    }
    return sem;
}

int Sem_wait(sem_t *sem)
{
    while (sem_wait(sem) == -1)
    {
        if (errno == EINTR)
        {
            continue;
        }
        fprintf(stderr, "Ошибка sem_wait: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return 0;
}

int Sem_unlink(const char *name)
{
    if (sem_unlink(name) == -1)
    {
        if (errno == ENOENT)
            return 0;
        fprintf(stderr, "Ошибка sem_unlink: для имени '%s': %s\n", name, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return 0;
}

int Sem_post(sem_t *sem)
{
    if (sem_post(sem) == -1)
    {
        fprintf(stderr, "Ошибка sem_post: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return 0;
}

int Sem_close(sem_t *sem)
{
    if (sem_close(sem) == -1)
    {
        fprintf(stderr, "Ошибка sem_close: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return 0;
}

int Shm_open(const char *name, int oflag, mode_t mode)
{
    if (name == NULL)
    {
        fprintf(stderr, "Ошибка: передан пустой указатель\n");
        exit(EXIT_FAILURE);
    }

    int fd_shm = shm_open(name, oflag, mode);
    if (fd_shm == -1)
    {
        fprintf(stderr, "Ошибка shm_open для '%s': %s\n", name, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return fd_shm;
}

int Shm_unlink(const char *name)
{
    if (shm_unlink(name) == -1)
    {
        if (errno == ENOENT)
            return 0;
        fprintf(stderr, "Ошибка shm_unlink: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return 0;
}

int Ftruncate(int fd, off_t length)
{
    if (ftruncate(fd, length) == -1)
    {
        fprintf(stderr, "Ошибка ftruncate для fd = %d (размер %jd): %s\n", fd, (intmax_t)length, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return 0;
}

void *Mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
    void *ptr = mmap(addr, length, prot, flags, fd, offset);
    if (ptr == MAP_FAILED)
    {
        fprintf(stderr, "Ошибка mmap (длина %lu, fd %d): %s\n", (unsigned long)length, fd, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return ptr;
}

int Munmap(void *addr, size_t length)
{
    if (munmap(addr, length) == -1)
    {
        fprintf(stderr, "Ошибка munmap для адреса %p (длина %lu): %s\n", addr, (unsigned long)length, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return 0;
}