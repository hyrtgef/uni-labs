#include <signal.h>
#include <fcntl.h> 
#include <errno.h>
#include <dlfcn.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fifo_hdr.h"
#include "utils.h"

typedef int (*process_fn) (const char*, size_t, char*, size_t, size_t*, long ,long*);

static int fd_req = -1;
static int fd_res = -1;
static void *handle = NULL;

void handle_signal(int sig)
{
    (void)sig;
    const char msg[] = "\nПолучен SIGINT, корректное завершение...\n";

    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
    Unlink(REQ_FIFO);
    Unlink(RES_FIFO);
    
    if(handle != NULL) dlclose(handle);
    if(fd_req != -1) Close(fd_req);
    if(fd_res != -1) Close(fd_res);
    
    exit(0);
}


int main(void)
{
    struct sigaction sa;
    memset(&sa, 0 , sizeof(sa));
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if(sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction");
        return 1;
    }
    
    printf("\nСервер запущен\n");

    Unlink(REQ_FIFO);
    Unlink(RES_FIFO);

    Mkfifo(REQ_FIFO, 0666);
    Mkfifo(RES_FIFO, 0666);

    fd_req = Open(REQ_FIFO, O_RDONLY);
    fd_res = Open(RES_FIFO, O_WRONLY);

    handle = dlopen("./libprocess.so", RTLD_LAZY);
    if(handle == NULL)
    {
        fprintf(stderr, "dlopen: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    process_fn process_chunk =(process_fn)dlsym(handle, "process_chunk");
    if(process_chunk == NULL)
    {
        fprintf(stderr, "dlsym: %s\n", dlerror());
        dlclose(handle);
        exit(EXIT_FAILURE);
    }

    fifo_msg_t msg;
    while(1)
    {
        ssize_t n = Read(fd_req, &msg, sizeof(msg));
        
        if (n == 0)
        {
            Close(fd_req);
            fd_req = Open(REQ_FIFO, O_RDONLY);
            continue;
        }
        
        if (n != (ssize_t)sizeof(msg))
        {
            continue;
        }

        char out[BUF_SIZE];
        size_t out_len;
        long count = 0;

        if (process_chunk(msg.data, msg.in_len, out, BUF_SIZE,
                          &out_len, msg.limit, &count) == -1)
        {
            fprintf(stderr, "Ошибка обработки: переполнение буфера\n");
            out_len = 0;
            count = -1;
        }

        memcpy(msg.data, out, out_len);
        msg.in_len = out_len;
        msg.count = count;

        Write(fd_res, &msg, sizeof(msg));
    }
    dlclose(handle);
    
    return 0;
}