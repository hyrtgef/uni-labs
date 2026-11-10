#ifndef UTILS_H
#define UTILS_H

#include <sys/types.h>

int Open(const char *pethname, int flags, ...);

ssize_t Read(int fd, void* buf, size_t count);

ssize_t Write(int fd, const void* buf, size_t count);

int Close(int fd);

#endif