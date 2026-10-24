#ifndef MQ_HDR_H
#define MQ_HDR_H

#include "utils.h"

#define REQ_QUEUE "/req_queue"
#define RES_QUEUE "/res_queue"

#define MAX_MSG 10

typedef struct mq_msg_t
{
    size_t in_len;
    long limit;
    long count;
    char data[BUF_SIZE];
}mq_msg_t;

#endif