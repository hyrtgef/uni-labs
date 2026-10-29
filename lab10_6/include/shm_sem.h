#ifndef SHM_SEM_H
#define SHM_SEM_H

#include "utils.h"

#define SHM_NAME "/process_shm"
#define SEM_REQ "/sem_req"
#define SEM_RES "/sem_res"

typedef struct
{
    size_t in_len;
    long limit;
    long count;
    char data[BUF_SIZE];
} shm_msg_t;

#endif