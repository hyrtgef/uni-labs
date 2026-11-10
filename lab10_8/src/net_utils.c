#include "net_utils.h"

#include <errno.h>
#include <string.h>

int Socket(int domain, int type, int protocol)
{
    int server_fd = socket(domain, type, protocol);
    if (server_fd == -1)
    {
        printf("Ошибка socket: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return server_fd;
}

void Bind(int fd, const struct sockaddr *addr, socklen_t len)
{
    if (bind(fd, addr, len) == -1)
    {
        fprintf(stderr, "Ошибка bind: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void Listen(int fd, int backlog)
{
    if (listen(fd, backlog) == -1)
    {
        fprintf(stderr, "Ошибка listen: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

int Accept(int fd, struct sockaddr *addr, socklen_t *size)
{
    int client_fd = accept(fd, addr, size);
    if (client_fd == -1)
    {
        fprintf(stderr, "Ошибка accept: %s\n", strerror(errno));
        return -1;
    }
    return client_fd;
}

void Connect(int fd, const struct sockaddr *addr, socklen_t len)
{
    if (connect(fd, addr, len) == -1)
    {
        fprintf(stderr, "Ошибка connect: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

ssize_t Send_all(int fd, const void *buf, size_t size)
{
    if (buf == NULL)
    {
        fprintf(stderr, "Ошибка: буфер пуст\n");
        return -1;
    }
    if (fd < 0)
    {
        fprintf(stderr, "Ошибка: некорректный fd\n");
        return -1;
    }
    if (size == 0)
        return 0;

    ssize_t total = 0;
    while (total < (ssize_t)size)
    {
        ssize_t n = send(fd, (const char *)buf + total, size - total, MSG_NOSIGNAL);
        if (n == -1)
        {
            if (errno == EINTR)
                continue;
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                continue;
            perror("send");
            return -1;
        }
        if (n == 0)
            return total;   /* соединение закрыто */
        total += n;
    }
    return total;
}

ssize_t Recv_all(int fd, void *buf, size_t len)
{
}

void Close_socket(int fd)
{
    if (close(fd) == -1)
    {
        fprintf(stderr, "Ошибка close_socket: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void Setsockopt_reuse(int fd)
{
    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        fprintf(stderr, "Ошибка setsockopt: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void Inet_pton(int af, const char *src, void *dst)
{
    int res = inet_pton(af, src, dst);
    if (res <= 0)
    {
        if (res == 0)
        {
            fprintf(stderr, "Ошибка inet_pton неверный формат сетевого адреса: %s\n", src);
            exit(EXIT_FAILURE);
        }
        else
        {
            fprintf(stderr, "Ошибка inep_pton: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
}

const char *Inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
    const char *result = inet_ntop(af, src, dst, size);
    if (result == NULL)
    {
        fprintf(stderr, "Ошибка inet_ntop: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return result;
}