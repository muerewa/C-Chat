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
    int pthcount; // номер пользователя
    int fd; // файловый дескриптор
    struct users *user; // структура пользователя
}; // Аргументы для функции Connection

struct users {
    int fd; // файловый дескриптор
    int msgCount; // счетчик сообщений
    char *name; // имя пользователя
};

void *Connection(void *argv) {

    while (true) {
        char buffer[256];

        int fd = ((struct args*)argv)->fd; // Достаем файловый дескриптор из аргументов

        int pthcount = ((struct args*)argv)->pthcount; // Достаем номер пользователя
        int valread = read(fd, buffer, 255); // Читаем сообщение
        struct users *user = ((struct args*)argv)->user; // Достаем структуру юзера

        buffer[strlen(buffer) - 1] = '\0';

        if(valread != 0) { // Слушаем сообщения
            char newBuffer[255]; // Создаем буффер

            if(!user->name) {
                user->name = strdup(buffer); // Если нет имени пользователя, то создаем его
            }
            strcpy(newBuffer, user->name); // Добавляем имя в буффер
            if (user->msgCount == 0) {
                strcat(newBuffer, " joined chat\n"); // Если первое сообщение то выводим сообщение о присоединении
            } else {
                strcat(newBuffer, ": ");
                strcat(newBuffer, buffer); // Добавляем в буффер сообщение
            }

            for (int i = 0; i < count; ++i) { // Проходимся по массиву сокетов
                if(i != pthcount) {
                    send(users[i] , newBuffer, strlen(newBuffer) , 0 ); // Отправляем сообщение всем кроме нас
                }
            }
            user->msgCount = 1; // Делаем счетчик не равным нулю
        } else {
            char connBuffer[255];
            strcpy(connBuffer, user->name);
            strcat(connBuffer, " disconnected\n");
            for (int i = 0; i < count; ++i) { // Проходимся по массиву сокетов
                send(users[i] , connBuffer , strlen(connBuffer) , 0 ); // Отправляем сообщение всем кроме нас
            }
            users[pthcount] = 0;
            free(user->name); // Освобождаем имя в структуре
            free(user); // Освобождаем структуру
            pthread_exit(NULL); // Выходим из потока
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
            puts(buffer); // Выводим сообщение о присоединении в лог

            pthread_t thread_id = count; // создаем id потока
            int pthcount = count;
            struct args *Thread = (struct args *)malloc(sizeof(struct args)); // Инициализируем структуру аргументов

            Thread->pthcount = pthcount;
            Thread->fd = fd;

            struct users *user = malloc(sizeof(users)); // Инициализируем структуру
            user->fd = fd;
            user->msgCount = 0;

            Thread->user = user; // Передаем структуру в аргументы потока
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