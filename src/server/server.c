#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "unistd.h"
#include "../../include/RSA.h"
#include "../../include/shifre.h"
#include "../../include/wrappers.h"
#include <pthread.h>
#include "stdbool.h"
#include "string.h"
#include "stdlib.h"
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>

int users[30] = {0}; // Массив сокетов
char *nicknames[30] = {NULL};
int count = 0; // Счетчик пользователей

int *serverSocket = NULL;

struct keys key;

struct args {
    int pthcount; // номер пользователя
    int fd; // файловый дескриптор
    struct users *user; // структура пользователя
    struct keys *key;
}; // Аргументы для функции Connection

struct users {
    int fd; // файловый дескриптор
    int msgCount; // счетчик сообщений
    char *name; // имя пользователя
};

void getTime(char *dateTime) { // Получаем текущую время и дату
    time_t timer;
    struct tm* tm_info;

    timer = time(NULL);
    tm_info = localtime(&timer);

    strftime(dateTime, 26, "[%d-%m-%Y %H:%M:%S]", tm_info);
}

void printUserLogMsg(int fd, char *name, char *msg) { // Получаем ip и port пользователя
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    getpeername(fd, (struct sockaddr *)&addr, &addr_size);

    char time[50];
    getTime(time);

    if (name == "") {
        name = "\b";
    }

    printf("%s (%s:%d) %s %s\n", time, inet_ntoa(addr.sin_addr), htons(addr.sin_port), name, msg);
    fflush(stdout);
}

void printServerLogMsg(char *msg, bool removeSignal) {
    char time[50];
    getTime(time);

    if (removeSignal) {
        printf("\b\b");
    }

    printf("%s %s\n", time, msg);
    fflush(stdout);
}

void intHandler(int dummy) {
    close(*serverSocket);
    printServerLogMsg("Stopped server", true);
    exit(0);
}

void *Connection(void *argv) {
    while (true) {
        char buffer[256] = {0};

        int fd = ((struct args*)argv)->fd; // Достаем файловый дескриптор из аргументов

        int pthcount = ((struct args*)argv)->pthcount; // Достаем номер пользователя
        int valread = read(fd, buffer, 255); // Читаем сообщение
        struct users *user = ((struct args*)argv)->user; // Достаем структуру юзера
        buffer[strlen(buffer) - 1] = '\0';

        if(valread != 0) { // Слушаем сообщения

            user->name = user->msgCount == 0 ? strdup(buffer) : user->name;
            char *msgBuff = user->msgCount == 0 ? " joined chat\n" : buffer;
            char newBuffer[strlen(user->name) + strlen(msgBuff) + 5]; // Создаем буффер

             // Добавляем имя в буффер
            if (user->msgCount == 0) {
                printUserLogMsg(fd, user->name, "joined chat");
                nicknames[pthcount] = user->name;
                strcpy(newBuffer, user->name);
                strcat(newBuffer, msgBuff); // Если первое сообщение то выводим сообщение о присоединении

            } else {
                strcpy(newBuffer, user->name);
                strcat(newBuffer, ": ");
                strcat(newBuffer, msgBuff); // Добавляем в буффер сообщение
                strcat(newBuffer, "\n\0");
            }

            for (int i = 0; i < count; ++i) { // Проходимся по массиву сокетов
                if(users[i] != fd && users[i] != 0 && nicknames[i] != NULL) {
                    send(users[i] , newBuffer, strlen(newBuffer) , 0 ); // Отправляем сообщение всем кроме нас
                }
            }
            user->msgCount = 1;
        } else {
            char connBuffer[strlen(" disconnected\n") + strlen(user->name)];

            strcpy(connBuffer, user->name);
            strcat(connBuffer, " disconnected\n");

            if(user->msgCount != 0) {
                printUserLogMsg(fd, user->name, "disconnected");
            } else {
                printUserLogMsg(fd, "", "disconnected");
            }

            if (user->msgCount != 0) {
                for (int i = 0; i < count; ++i) { // Проходимся по массиву сокетов
                    send(users[i] , connBuffer , strlen(connBuffer) , 0 ); // Отправляем сообщение всем кроме нас
                }
            }

            users[pthcount] = 0;
            nicknames[pthcount] = NULL;
            free(user); // Освобождаем структуру
            pthread_exit(NULL); // Выходим из потока
        }
    }
}

void ConnLoop(int server, struct sockaddr *addr, socklen_t *addrlen) {
    while (true) {
        if(count <= 30) {
            int fd = Accept(server, addr, addrlen); // Принимаем новое подключение
            users[count] = fd; // Добавляем в массив дескриптор
            pthread_t thread_id = count; // создаем id потока
            int pthcount = count;
            ++count;

            printUserLogMsg(fd, "", "connected");

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
    generateKeys(&key);
    signal(SIGINT, intHandler);

    int server = Socket(AF_INET, SOCK_STREAM, 0); // Создаем сокет
    serverSocket = &server;

    struct sockaddr_in addr = {0}; // Создаем адресс сокета
    addr.sin_family = AF_INET; // Семейство протоколов(ipv4)
    addr.sin_port = htons(3030); // Порт

    Bind(server, (const struct sockaddr *) &addr, sizeof addr); // Привязываем к сокету адресс
    Listen(server, 5); // Прослушваем(5 человек максимум могут находиться в очереди)

    socklen_t addrlen = sizeof addr; // Размер адреса

    ConnLoop(server, (struct sockaddr*) &addr, &addrlen); // Запускаем принятие пользователей
    return 0;
}
