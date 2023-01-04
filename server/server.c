#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "unistd.h"
#include "../wrapper/wrappers.h"
#include <pthread.h>
#include "stdbool.h"
#include "string.h"
#include "stdlib.h"

int users[30] = {0}; // Массив сокетов
int count = 0; // Счетчик пользователей

struct args {
    int pthcount;
    int fd;
}; // Аргументы для функции Connection

void *Connection(void *argv) {

    while (true) {
        char buffer[256];
        int fd = ((struct args*)argv)->fd; // Достаем файловый дескриптор из аргументов
        int pthcount = ((struct args*)argv)->pthcount;
        int valread = read(fd, buffer, 255);
        if(valread != 0) { // Слушаем сообщения
            buffer[valread] = '\0';
            for (int i = 0; i < count; ++i) { // Проходимся по массиву сокетов
                if(users[i] != fd) {
                    send(users[i] , buffer , strlen(buffer) , 0 ); // Отправляем сообщение всем кроме нас
                }
            }
        } else {
            char connBuff[] = "disconnect";
            for (int i = 0; i < count; ++i) { // Проходимся по массиву сокетов
                send(users[i] , connBuff , strlen(connBuff) , 0 ); // Отправляем сообщение всем кроме нас
            }
            users[pthcount] = 0;
            pthread_exit(NULL);
        }
    }
}

void ConnLoop(int server, struct sockaddr *addr, socklen_t *addrlen) {

    while (true) {
        if(count <= 30) {
            int fd = Accept(server, addr, addrlen); // Принимаем новое подключение
            users[count++] = fd; // Добавляем в массив дескриптор
            char buffer[30];
            sprintf(buffer, "Connection %d", count);
            puts(buffer);
            pthread_t thread_id = count;
            int pthcount = count;
            struct args *Thread = (struct args *)malloc(sizeof(struct args)); // Создаем структуру аргументов
            Thread->pthcount = pthcount;
            Thread->fd = fd;
            pthread_create(&thread_id, NULL, Connection, (void *)Thread); // Создаем отдельный поток для каждого пользователя
        }
    }
}

int main() {
    int server = Socket(AF_INET, SOCK_STREAM, 0); // Создаем сокет

    struct sockaddr_in addr = {0}; // Создаем адресс сокета
    addr.sin_family = AF_INET; // Семейство протоколов(ipv4)
    addr.sin_port = htons(3030); // Порт

    Bind(server, (const struct sockaddr *) &addr, sizeof addr); // Привязываем к сокету адресс
    Listen(server, 5); // Прослушваем(5 человек максимум могут находиться в очереди)

    socklen_t addrlen = sizeof addr; // Размер адреса
    ConnLoop(server, (struct sockaddr*) &addr, &addrlen); // Запускаем принятие пользователей

    return 0;
}


