#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stddef.h>

int send_data(int fd, const char *payload, size_t len, long limit); // отправить данные + лимит
int recv_data(int fd, char **payload, size_t *len, long *limit);    // принять данные + лимит

int send_result(int fd, long count);  // отправить результат
int recv_result(int fd, long *count); // принять результат

int send_error(int fd, int code, const char *msg);        // отправить ошибку
int recv_error(int fd, int *code, char *msg, size_t cap); // принять ошибку

#endif