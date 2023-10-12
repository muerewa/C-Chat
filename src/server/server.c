#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "unistd.h"
#include <pthread.h>
#include "stdbool.h"
#include "string.h"
#include "stdlib.h"
#include <signal.h>

#include "../../include/connection.h"
#include "../../include/RSA.h"
#include "../../include/wrappers.h"
#include "../../include/log.h"
#include "../../include/serverHandlers.h"
#include "../../include/msgHandlers.h"


#define MSGLEN 2048

struct users* usersArr[30] = {NULL}; // Массив сокетов
char *nicknames[30] = {NULL};

struct users* privateUsersArr[30] = {NULL}; // Массив сокетов
char *privateNicknames[30] = {NULL};

int count = 0; // Счетчик пользователей
int privateCount = 0;

struct keys key;

int *serverSocket = NULL;

/**
 * @brief
 *
 * @param dummy
 */
void intHandler(int dummy) {
    close(*serverSocket);
    printServerLogMsg("Stopped server", true);
    EVP_PKEY_free(key.pubKey);
    EVP_PKEY_free(key.privKey);
    exit(0);
}

int roomHandler(char *roomType, struct args *Thread, int *pthcount, struct users *user) {
    if (strcmp(roomType,"public") == 0) {
        Thread->usersArr = usersArr;
        Thread->nicknames = nicknames;
        Thread->count = &count;
        *pthcount = newUser(usersArr);
        usersArr[*pthcount] = user;
        ++count;
        return 0;
    } else if (strcmp(roomType,"private") == 0) {
        Thread->usersArr = privateUsersArr;
        Thread->nicknames = privateNicknames;
        Thread->count = &privateCount;
        *pthcount = newUser(privateUsersArr);
        privateUsersArr[*pthcount] = user;
        ++privateCount;
        return 0;
    }
    return -1;
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

            printUserLogMsg(fd, "", "connected");

            struct args *Thread = (struct args *)malloc(sizeof(struct args)); // Инициализируем структуру аргументов

            Thread->fd = fd;
            Thread->key = &key;

            struct users *user = malloc(sizeof(struct users)); // Инициализируем структуру
            user->fd = fd;
            user->msgCount = 0;

            if (serverKeyHandler(user,&key,fd) == -1) {
                close(fd);
                free(user);
                free(Thread);
                continue;
            }

            int statcode = 0, valread, pthcount = 0;
            char *roomType = readMsgHandler(fd, &valread, &key, &statcode);

            if(roomHandler(roomType, Thread, &pthcount, user) == -1) {
                writeMsgHandler(user->fd,"Некорректное сообщение", user->pubKey);
                close(fd);
                free(user);
                free(Thread);
                continue;
            }

            Thread->pthcount = pthcount;
            user->msgCount = 0;
            Thread->user = user; // Передаем структуру в аргументы потока
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
            pthread_create(&thread_id, &attr, Connection, (void *)Thread); // Создаем отдельный поток для каждого пользователя
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
