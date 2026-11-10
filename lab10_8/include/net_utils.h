#ifndef NET_UTILS_H
#define NET_UTILS_H

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stddef.h>
#include <netinet/in.h>>
#include <stdio.h>
#include <stdlib.h>

int Socket(int domain, int type, int protocol);

void Bind(int fd, const struct sockaddr *addr, socklen_t len);

void Listen(int fd, int backlog);

int Accept(int fd, struct sockaddr *addr, socklen_t *len);

void Connect(int fd, const struct sockaddr *addr, socklen_t len);

ssize_t Send_all(int fd, const void *buf, size_t size);

ssize_t Recv_all(int fd, void *buf, size_t len);

void Close_socket(int fd);

void Setsockopt_reuse(int fd);

void Inet_pton(int af, const char *src, void *dst);

const char *Inet_ntop(int af, const void *src, char *dst, socklen_t size);

#endif