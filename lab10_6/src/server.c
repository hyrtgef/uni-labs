#include <fcntl.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <semaphore.h>

#include "shm_sem.h"
#include "utils.h"

typedef int (*process_fn)(const char *, size_t, char *, size_t, size_t *, long, long *);

static int shm_fd = -1;
static shm_msg_t *shm = NULL;
static sem_t *sem_req = NULL;
static sem_t *sem_res = NULL;
static void *handle = NULL;

void handle_signal(int)
{
    const char msg[] = "\n Получен SIGINT корректное завершение\n";

    (void)!write(STDOUT_FILENO, msg, sizeof(msg) - 1);

    if (shm != NULL)
    {
        Munmap(shm, sizeof(shm_msg_t));
    }
    if (shm_fd != -1)
    {
        Shm_unlink(SHM_NAME);
        Close(shm_fd);
    }
    if (sem_req != NULL)
    {
        Sem_close(sem_req);
        Sem_unlink(SEM_REQ);
    }
    if (sem_res != NULL)
    {
        Sem_close(sem_res);
        Sem_unlink(SEM_RES);
    }
    if (handle != NULL)
    {
        dlclose(handle);
    }

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

    Shm_unlink(SHM_NAME);
    Sem_unlink(SEM_REQ);
    Sem_unlink(SEM_RES);

    shm_fd = Shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    Ftruncate(shm_fd, sizeof(shm_msg_t));
    shm = Mmap(NULL, sizeof(shm_msg_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    sem_req = Sem_open(SEM_REQ, O_CREAT, 0666, 0);
    sem_res = Sem_open(SEM_RES, O_CREAT, 0666, 0);

    handle = dlopen("./lib/libprocess.so", RTLD_LAZY);
    if(handle == NULL)
    {
        fprintf(stderr, "Ошибка dlopen: %s\n", dlerror());
        handle_signal(0);
    }

    process_fn process_chunk = (process_fn)dlsym(handle, "process_chunk");
    if(process_chunk == NULL)
    {
        fprintf(stderr, "Ошибка dlsym: %s\n", dlerror());
        dlclose(handle);
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        Sem_wait(sem_req);
        
        char out[BUF_SIZE];
        size_t out_len = 0;
        long count = 0;

        if(process_chunk(shm->data, shm->in_len, out, BUF_SIZE, &out_len, shm->limit, &count) == -1)
        {
            out_len = 0; 
            count = -1;
        }
        
        memcpy(shm->data, out,out_len);
        shm->in_len = out_len;
        shm->count = count;
        Sem_post(sem_res);

    }
    dlclose(handle);
    return 0;   
}
