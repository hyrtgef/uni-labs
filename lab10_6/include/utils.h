#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <sys/types.h>
#include <semaphore.h>

#define BUF_SIZE 1024

int Open(const char *pathname, int flags, ...);
ssize_t Write(int fd, const void *buf, size_t count);
ssize_t Read(int fd, void *buf, size_t count);
int Close(int fd);
int LockRead(int fd);
int LockWrite(int fd);

// Семафоры
sem_t *Sem_open(const char *name, int oflag,
                mode_t mode, unsigned int value); // Создает новый или открывает существующий именованный семафор
int Sem_wait(sem_t *sem);                         // Захватывает семафор
int Sem_unlink(const char *name);                 // Удаляет имя семафора из системы
int Sem_post(sem_t *sem);                         // Освобождает семафор
int Sem_close(sem_t *sem);                        // Закрывает доступ

// Разделяемая память
int Shm_open(const char *name, int oflag, mode_t mode);                           // Создает или открывает объект разделяемой памяти
int Shm_unlink(const char *name);                                                 // Удаляет имя объекта разделяемой памяти из системы
int Ftruncate(int fd, off_t length);                                              // Задает или изменяет размер разделяемой памяти
void *Mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset); // Отображает объект разделяемой памяти в адресное пространство процесса
int Munmap(void *addr, size_t length);                                            // Отменяет отображение памяти

#endif