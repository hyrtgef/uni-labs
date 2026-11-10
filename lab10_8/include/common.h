#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

#define DEFAULT_PORT 9090      // порт по умолчанию
#define DEFAULT_IP "127.0.0.1" // localhost
#define BUF_SIZE 4096          // размер буффера

typedef enum
{
    MSG_DATA = 1,   // клиент -> сервер: данные + limit
    MSG_RESULT = 2, // сервер -> клиент:число вставок
    MSG_ERROR = 3   // сервер -> клиент: код + текст ошибки
} MessageType;

#define RESULT_OK 0
#define RESULT_ERROR -1

#define ERR_BAD_REQUEST 1 // некорректный заголовок
#define ERR_TOO_LARGE 2   // len > BUF_SIZE
#define ERR_PROCESS 3     // process_chunk вернул -1
#define ERR_IO 4          // ошибка чтения/записи

typedef struct
{
    uint32_t type; // Messagetype
    uint32_t len;  // длина тела в байтах
    int64_t limit; // лимит вставок
} MessageHeader;

#endif