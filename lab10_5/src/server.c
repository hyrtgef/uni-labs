#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <mqueue.h>
#include <dlfcn.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>

#include "mq_hdr.h"
#include "utils.h"

typedef int(*process_fn)(const char *, size_t, char *, size_t, size_t *, long, long *);

static mqd_t req_q = (mqd_t)-1;
static mqd_t res_q = (mqd_t)-1;
static void *handle = NULL;

void handle_signal(int)
{
    const char msg[] = "\n Получен SIGINT корректное завершение\n";

    (void)!write (STDOUT_FILENO, msg, sizeof(msg)-1);
    Mq_unlink(REQ_QUEUE);
    Mq_unlink(RES_QUEUE);

    if(handle != NULL) dlclose(handle);
    if(req_q != (mqd_t)-1) Mq_close(req_q);
    if(res_q != (mqd_t)-1) Mq_close(res_q);

    exit(0);
}

int main(void)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    if(sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    Mq_unlink(REQ_QUEUE);
    Mq_unlink(RES_QUEUE);

    struct mq_attr attr;
    memset(&attr, 0, sizeof(attr));
    attr.mq_maxmsg = MAX_MSG;
    attr.mq_msgsize = sizeof(mq_msg_t);
    attr.mq_flags = 0;

    req_q = Mq_open(REQ_QUEUE, O_CREAT | O_RDONLY, 0666, &attr);
    res_q = Mq_open(RES_QUEUE, O_CREAT | O_WRONLY, 0666, &attr);

    handle = dlopen("./lib/libprocess.so", RTLD_LAZY);
    if(handle == NULL)
    {
        fprintf(stderr, "dlopen: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    process_fn process_chunk = (process_fn)dlsym(handle, "process_chunk");
    if(process_chunk == NULL)
    {
        fprintf(stderr , "dlsym: %s\n", dlerror());
        dlclose(handle);
        exit(EXIT_FAILURE);
    }

    mq_msg_t msg = {0};

    while(1)
    {
        ssize_t n = Mq_receive(req_q , &msg, sizeof(msg), NULL);
        if(n != (ssize_t)sizeof(msg))
        {
            continue;  
        }

        char out[BUF_SIZE];
        size_t out_len = 0;
        long count = 0;

        if(process_chunk(msg.data, msg.in_len, out, BUF_SIZE, &out_len, msg.limit, &count) == -1)
        {
            out_len = 0;
            count = -1;
        }

        memcpy(msg.data, out, out_len);
        msg.in_len = out_len;
        msg.count = count;
        
        Mq_send(res_q, (char*)&msg, sizeof(msg), 0);

    }
    dlclose(handle);
    return 0;
}   