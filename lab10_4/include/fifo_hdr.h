#ifndef FIFO_HDR_H
#define FIFO_HDR_H

#include <stddef.h>
#include "utils.h"

#define REQ_FIFO "/tmp/process_req_fifo" // канал для запросов клиент сервер
#define REQ_FIFO "tmp/process_res_fifo" // канал для запросов сервер клиент

typedef struct 
{
    size_t in_len;
    long limit; 
    char data[BUF_SIZE];
}fifo_msg_t;


#endif