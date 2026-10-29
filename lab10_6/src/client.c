#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <limits.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "shm_sem.h"
#include "utils.h"

long parse_limit(const char *str)
{
    errno = 0;
    char *endptr = NULL;
    long limit = strtol(str, &endptr, 10);
    if (errno == ERANGE || str == endptr || *endptr != '\0')
    {
        fprintf(stderr, "Ошибка не корректное число\n");
        exit(EXIT_FAILURE);
    }

    if (limit < 0)
    {
        fprintf(stderr, "Ошибка лимит не может быть меньше 0\n");
        exit(EXIT_FAILURE);
    }

    return limit;
}

int read_input_file(shm_msg_t *msg, const char *pathname)
{
    int fd_in = Open(pathname, O_RDONLY);

    LockRead(fd_in);

    ssize_t n = Read(fd_in, msg->data, BUF_SIZE);

    msg->in_len = n;

    Close(fd_in);
    return 0;
}

int write_output_file(const shm_msg_t *msg, const char *pathname)
{
    int fd_out = Open(pathname, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    Write(fd_out, msg->data, msg->in_len);

    Close(fd_out);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        fprintf(stderr, "Ошибка %s input.txt output.txt N", argv[0]);
        return -1;
    }

    long limit = parse_limit(argv[argc - 1]);

    shm_msg_t msg = {0};
    read_input_file(&msg, argv[1]);
    msg.limit = limit;

    int shm_fd = Shm_open(SHM_NAME, O_RDWR, 0);
    shm_msg_t *shm = Mmap(NULL, sizeof(shm_msg_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    sem_t *sem_req = Sem_open(SEM_REQ, 0, 0, 0);
    sem_t *sem_res = Sem_open(SEM_RES, 0, 0, 0);

    memcpy(shm->data, msg.data, msg.in_len);
    shm->in_len = msg.in_len;
    shm->limit = msg.limit;

    Sem_post(sem_req);
    Sem_wait(sem_res);

    memcpy(msg.data, shm->data, shm->in_len);
    msg.in_len = shm->in_len;
    msg.limit = shm->limit;
    msg.count = shm->count;

    write_output_file(&msg, argv[2]);
    Munmap(shm, sizeof(shm_msg_t));
    Close(shm_fd);
    Sem_close(sem_req);
    Sem_close(sem_res);

    printf("Вставлено точек: %ld\n", msg.count);
    return 0;
}