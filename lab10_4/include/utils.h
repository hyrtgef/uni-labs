#ifndef UTILS_H
#define UTILS_H

#include <stdarg.h>
#include <sys/types.h>

#define BUF_SIZE 1024

int Open(const char *pathname, int flags, ...);

ssize_t Write(int fd, const void *buf, size_t count);

ssize_t Read(int fd, void *buf, size_t count);

int Close(int fd);

int LockRead(int fd);

int LockWrite(int fd);

int Mkfifo(const char *pathname, mode_t mode);

int Unlink(const char *pathname);

#endif