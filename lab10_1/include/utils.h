#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <sys/types.h>


#define BUF_SIZE 1024

/// @brief Открывает файл по указанному пути с заданными флагами
/// @param pathname Путь к файлу
/// @param flags Флаги открытия 
/// @param mode_t при O_CREATE
/// @return  Файловый дескриптор при успехе, -1 при ошибке
int Open(const char *pathname, int flags, ...);

/// @brief Читает данные из файла в буфер
/// @param fd Файловый дескриптор
/// @param buf Буфер для записи прочитанных данных
/// @param count Максимальное число байт для чтения
/// @return Число прочитанных байт, -1 при ошибке
ssize_t Read(int fd, void* buf, size_t count);

/// @brief Записывает данные из буфера в файл
/// @param fd Файловый дескриптор
/// @param buf Буфер с данными для записи
/// @param count Число байт для записи
/// @return Число записанных байт, -1 при ошибке
ssize_t Write(int fd, const char *buf, size_t count);

/// @brief Закрывает файловый дескриптор
/// @param fd Файловый дескриптор
/// @return 0 при успехе, -1 при ошибке
int Close(int fd);

/// @brief Выводит сообщение об ошибке в поток stderr с добавлением errno
/// @param msg Текст сообщения об ошибке
void print_error(const char* msg);

/// @brief shared lock разделяемая блокировка для чтения на указатель дескриптора файла
/// @param fd Дескриптор открытого файла 
/// @return Возвращает 0 при успешном выполнении или -1 в случае ошибки
int LockRead(int fd);

/// @brief  exclucive lock устанавливает монопольную блокировку для записи на указатель дескриптора файла
/// @param fd Дескриптор открытого файла
/// @return Возвращает 0 при успешном выполнении или -1 в случае ошибки
int LockWrite(int fd);

#endif