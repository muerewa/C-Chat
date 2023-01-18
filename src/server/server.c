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
#include "../../include/log.h"
#include <signal.h>
#include "../../include/handlers.h"
#include "../../include/structures.h"

struct users usersArr[30] = {}; // Массив сокетов
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


/**
 * @brief 
 * 
 * @param dummy 
 */
void intHandler(int dummy) {
    close(*serverSocket);
    printServerLogMsg("Stopped server", true);
    exit(0);
}


/**
 * @brief 
 * 
 * @param argv 
 * @return void* 
 */
void *Connection(void *argv) {
    while (true) {
        char buffer[256] = {0};
        long encMsg[256] = {0};
        size_t encMsgLen = sizeof(encMsg)/sizeof(encMsg[0]);
        int fd = ((struct args*)argv)->fd; // Достаем файловый дескриптор из аргументов

        int pthcount = ((struct args*)argv)->pthcount; // Достаем номер пользователя
        int valread = read(fd, encMsg, encMsgLen); // Читаем сообщение

        decrypt(encMsg, encMsgLen, buffer, key.d, key.n);

        struct users *user = ((struct args*)argv)->user; // Достаем структуру юзера
        buffer[strlen(buffer) - 1] = '\0';

        if(valread != 0) { // Слушаем сообщения

            user->name = user->msgCount == 0 ? strdup(buffer) : user->name;
            char *msgBuff = user->msgCount == 0 ? " joined chat\n" : buffer;
            char newBuffer[strlen(user->name) + strlen(msgBuff) + 5]; // Создаем буффер
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
                if(usersArr[i].fd != fd && nicknames[i] != NULL) {
                    long encMsg[256] = {0};
                    encrypt(newBuffer, encMsg, usersArr[i].e, usersArr[i].n);
                    encMsgLen = sizeof(encMsg)/sizeof(encMsg[0]);
                    write(usersArr[i].fd , encMsg, encMsgLen); // Отправляем сообщение всем кроме нас
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
                    if(usersArr[i].fd != fd && nicknames[i] != NULL) {
                        long encMsg[256] = {0};
                        encrypt(connBuffer, encMsg, usersArr[i].e, usersArr[i].n);
                        encMsgLen = sizeof(encMsg)/sizeof(encMsg[0]);
                        write(usersArr[i].fd , encMsg , encMsgLen); // Отправляем сообщение всем кроме нас
                    }
                }
            }
            fflush(stdout);
            nicknames[pthcount] = NULL;
            free(user); // Освобождаем структуру
            pthread_exit(NULL); // Выходим из потока
        }
    }
}


/**
 * @brief 
 * 
 * @param server 
 * @param addr 
 * @param addrlen 
 */
void ConnLoop(int server, struct sockaddr *addr, socklen_t *addrlen) {
    while (true) {
        if(count <= 30) {
            int fd = Accept(server, addr, addrlen); // Принимаем новое подключение
            pthread_t thread_id = count; // создаем id потока
            int pthcount = count;

            printUserLogMsg(fd, "", "connected");

            struct args *Thread = (struct args *)malloc(sizeof(struct args)); // Инициализируем структуру аргументов

            Thread->pthcount = pthcount;
            Thread->fd = fd;

            struct users *user = malloc(sizeof(struct users)); // Инициализируем структуру
            user->fd = fd;
            user->msgCount = 0;

            serverKeyHandler(user,&key,fd);

            user->msgCount = 0;
            Thread->user = user; // Передаем структуру в аргументы потока
            usersArr[count] = *user; // Добавляем в массив дескриптор
            ++count;
            pthread_create(&thread_id, NULL, Connection, (void *)Thread); // Создаем отдельный поток для каждого пользователя
        }
    }
}


/**
 * @brief 
 * 
 * @return int 
 */
int main(int argc, char **argv) {
    printf("Generating keys...\n");
    fflush(stdout);

    generateKeys(&key);

    printf("done generating keys\n");
    fflush(stdout);

    printf("starting server...\n");
    fflush(stdout);

    signal(SIGINT, intHandler);

    int server = Socket(AF_INET, SOCK_STREAM, 0); // Создаем сокет
    serverSocket = &server;

    struct sockaddr_in addr = {0}; // Создаем адресс сокета
    addr.sin_family = AF_INET; // Семейство протоколов(ipv4)
    addr.sin_port = htons(3030); // Порт

    Bind(server, (const struct sockaddr *) &addr, sizeof addr); // Привязываем к сокету адресс
    Listen(server, 5); // Прослушваем(5 человек максимум могут находиться в очереди)

    socklen_t addrlen = sizeof addr; // Размер адреса

    printf("server running\n");
    fflush(stdout);

    ConnLoop(server, (struct sockaddr*) &addr, &addrlen); // Запускаем принятие пользователей
    return 0;
}
