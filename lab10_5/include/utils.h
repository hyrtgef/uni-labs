#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <sys/types.h>
#include <mqueue.h>

#define BUF_SIZE 1024

int Open(const char *pathname, int flags, ...);

ssize_t Write(int fd, const void *buf, size_t count);

ssize_t Read(int fd, void *buf, size_t count);

int Close(int fd);

int LockRead(int fd);

int LockWrite(int fd);

mqd_t Mq_open(const char *name, int oflag, mode_t mode, struct mq_attr *attr);

int Mq_send(mqd_t mqdes, const void *msg_ptr, size_t msg_len, unsigned int msg_prio);

ssize_t Mq_receive(mqd_t mqdes, void *msg_ptr, size_t msg_len, unsigned int *msg_prio);

int Mq_close(mqd_t mqdes);

int Mq_unlink(const char *name);

#endif