#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>

#include "utils.h"
#include "fifo_hdr.h"

long parse_limit(const char *str)
{
    errno = 0;
    char *endptr = NULL;
    long limit = strtol(str, &endptr, 10);
    if (*endptr != '\0' || errno == ERANGE || endptr == str)
    {
        fprintf(stderr, "Ошибка число не является корректным числом\n");
        exit(EXIT_FAILURE);
    }

    if (limit < 0)
    {
        fprintf(stderr, "Ошибка лимит не может быть отрицательным\n");
        exit(EXIT_FAILURE);
    }
    return limit;
}

int read_input_file(fifo_msg_t *msg, const char *pathname)
{
    if (pathname == NULL)
    {
        fprintf(stderr, "Ошибка имя не может быть NULL\n");
        exit(EXIT_FAILURE);
    }

    int fd_in = Open(pathname, O_RDONLY);
    if (fd_in == -1)
    {
        exit(EXIT_FAILURE);
    }

    if (LockRead(fd_in) == -1)
    {
        exit(EXIT_FAILURE);
    }

    ssize_t n;
    if ((n = Read(fd_in, msg->data, BUF_SIZE)) == -1)
    {
        fprintf(stderr, "Ошибка чтения файла\n");
        exit(EXIT_FAILURE);
    }
    msg->in_len = n;
    Close(fd_in);
    return 0;
}

int send_request(int fd, const fifo_msg_t *msg)
{
    if (Write(fd, msg, sizeof(fifo_msg_t)) == -1)
    {
        return -1;
    }
    return 0;
}

int receive_response(int fd, fifo_msg_t *msg)
{
    if (Read(fd, msg, sizeof(fifo_msg_t)) == -1)
    {
        return -1;
    }
    return 0;
}

int write_output_file(const fifo_msg_t *msg, const char *pathname)
{
    int fd_out = Open(pathname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_out == -1)
    {
        return -1;
    }

    if (LockWrite(fd_out) == -1)
    {
        return -1;
    }

    if (Write(fd_out, msg->data, msg->in_len) == -1)
    {
        return -1;
    }

    Close(fd_out);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        fprintf(stderr, "Ошибка ./client input.txt output.txt N\n");
        return -1;
    }

    long limit = parse_limit(argv[3]);

    fifo_msg_t msg = {0};

    if (read_input_file(&msg, argv[1]) == -1)
    {
        return -1;
    }
    msg.limit = limit;

    int fd_req = Open(REQ_FIFO, O_WRONLY);
    if (send_request(fd_req, &msg) == -1)
    {
        Close(fd_req);
        return -1;
    }
    Close(fd_req);

    int fd_res = Open(RES_FIFO, O_RDONLY);
    if (receive_response(fd_res, &msg) == -1)
    {
        Close(fd_res);
        return -1;
    }
    Close(fd_res);

    if (write_output_file(&msg, argv[2]) == -1)
    {
        return -1;
    }
    printf("Вставлено точек: %ld\n", msg.count);
    return 0;
}