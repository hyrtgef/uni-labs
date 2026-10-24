#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <mqueue.h>

#include "utils.h"
#include "mq_hdr.h"

long parse_limit(const char *str)
{
    errno = 0;
    char *endptr = NULL;
    long limit = strtol(str, &endptr, 10);
    if (*endptr != '\0' || endptr == str || errno == ERANGE)
    {
        fprintf(stderr, "Ошибка некорректное число\n");
        exit(EXIT_FAILURE);
    }
    if (limit < 0)
    {
        fprintf(stderr, "Ошибка лимит не может быть отрицательным\n");
        exit(EXIT_FAILURE);
    }

    return limit;
}

int read_input_file(mq_msg_t *msg, const char *pathname)
{
    int in_fd = Open(pathname, O_RDONLY);

    LockRead(in_fd);

    ssize_t n = Read(in_fd, msg->data, BUF_SIZE);

    msg->in_len = n;
    Close(in_fd);

    return 0;
}

int write_output_file(const mq_msg_t *msg, const char *pathname)
{
    int out_fd = Open(pathname, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    LockWrite(out_fd);

    Write(out_fd, msg->data, msg->in_len);

    Close(out_fd);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        fprintf(stderr, "Ошибка %s input.txt output.txt N\n", argv[0]);
        return -1;
    }

    long limit = parse_limit(argv[argc - 1]);

    mq_msg_t msg = {0};
    read_input_file(&msg, argv[1]);

    msg.limit = limit;

    mqd_t req_q = Mq_open(REQ_QUEUE, O_WRONLY, 0, NULL);
    mqd_t res_q = Mq_open(RES_QUEUE, O_RDONLY, 0, NULL);

    Mq_send(req_q, &msg, sizeof(msg), 0);

    ssize_t n = Mq_receive(res_q, &msg, sizeof(msg), NULL);
    if (n != (ssize_t)sizeof(msg))
    {
        fprintf(stderr, "Ошибка: неполное сообщение\n");
        Mq_close(req_q);
        Mq_close(res_q);
        return -1;
    }

    write_output_file(&msg, argv[2]);

    Mq_close(req_q);
    Mq_close(res_q);

    printf("Вставлено точек: %ld\n", msg.count);

    return 0;
}