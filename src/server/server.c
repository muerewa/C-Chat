#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "unistd.h"
#include <pthread.h>
#include "stdbool.h"
#include "string.h"
#include "stdlib.h"
#include <signal.h>

#include "../../include/RSA.h"
#include "../../include/wrappers.h"
#include "../../include/log.h"
#include "../../include/serverHandlers.h"
#include "../../include/msgHandlers.h"

#define MSGLEN 2048

struct users* usersArr[30] = {NULL}; // Массив сокетов
char *nicknames[30] = {NULL};
int count = 0; // Счетчик пользователей

pthread_mutex_t mutex;

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

int newUser() {
    int i = 0;
    while (usersArr[i] != NULL) {
        i++;
    }
    return i;
}

/**
 * @brief 
 * 
 * @param argv 
 * @return void* 
 */
void *Connection(void *argv) {
    while (true) {

        int fd = ((struct args*)argv)->fd; // Достаем файловый дескриптор из аргументов

        int pthcount = ((struct args*)argv)->pthcount; // Достаем номер пользователя

        int statcode = 0;
        int valread;
        char *buffer = readMsgHandler(fd, &valread, &key, &statcode);
        struct users *user = ((struct args*)argv)->user; // Достаем структуру юзера

        if (statcode == -1) {
            writeMsgHandler(user->fd,"Некорректное сообщение", user->pubKey);
            continue;
        }

        if(valread > 0) {

            user->name = user->msgCount == 0 ? strdup(buffer) : user->name;
            char *msgBuff = user->msgCount == 0 ? " joined chat" : buffer;
            char *Answer = (char *) malloc(strlen(user->name) + strlen(msgBuff) + 5); // Создаем буффер

            if (user->msgCount == 0) {

                char *msg = WelcomeMsg(user->name);
                writeMsgHandler(fd, msg, user->pubKey);
                free(msg);

                printUserLogMsg(fd, user->name, "joined chat");
                pthread_mutex_lock(&mutex);
                nicknames[pthcount] = user->name;
                pthread_mutex_unlock(&mutex);
                strcpy(Answer, user->name);
                strcat(Answer, msgBuff);
            } else {
                MsgBufferHandler(Answer, user->name, msgBuff);
            }

            for (int i = 0; i < count; ++i) { // Проходимся по массиву сокетов
                if(nicknames[i] != NULL && usersArr[i]->fd != fd) {
                    pthread_mutex_lock(&mutex);
                    writeMsgHandler(usersArr[i]->fd, Answer, usersArr[i]->pubKey);
                    pthread_mutex_unlock(&mutex);
                }
            }
            user->msgCount = 1;
            free(Answer);
            free(buffer);
        } else {
            int s = strlen(" disconnected");
            char *disbuffer = malloc( s + 1);

            if(user->msgCount != 0) {
                disbuffer = realloc(disbuffer, s + 1 + strlen(user->name));
                strcpy(disbuffer, user->name);
                strcat(disbuffer, " disconnected");
                printUserLogMsg(fd, user->name, "disconnected");
                pthread_mutex_lock(&mutex);
                free(user->name);
                pthread_mutex_unlock(&mutex);
            } else {
                char *usr = "user";
                disbuffer = realloc(disbuffer, strlen(usr) + 1 + s);
                strcat(disbuffer, " disconnected");
                printUserLogMsg(fd, "", "disconnected");
            }

            if ((user->msgCount != 0) && count >= 1) {
                for (int i = 0; i < count; ++i) { // Проходимся по массиву сокетов
                    if(nicknames[i] != NULL && usersArr[i]->fd != fd) {
                        pthread_mutex_lock(&mutex);
                        writeMsgHandler(usersArr[i]->fd, disbuffer, usersArr[i]->pubKey);
                        pthread_mutex_unlock(&mutex);
                    }
                }
            }
            pthread_mutex_lock(&mutex);
            fflush(stdout);
            nicknames[pthcount] = NULL;
            usersArr[pthcount] = NULL;
            free(user->pubKey);
            free(user); // Освобождаем структуру
            free(disbuffer);
            free(argv);
            pthread_mutex_unlock(&mutex);
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
            int pthcount = newUser();

            printUserLogMsg(fd, "", "connected");

            struct args *Thread = (struct args *)malloc(sizeof(struct args)); // Инициализируем структуру аргументов

            Thread->pthcount = pthcount;
            Thread->fd = fd;

            struct users *user = malloc(sizeof(struct users)); // Инициализируем структуру
            user->fd = fd;
            user->msgCount = 0;

            int res = serverKeyHandler(user,&key,fd);
            if (res == -1) {
                close(fd);
                free(user);
                free(Thread);
                continue;
            }

            user->msgCount = 0;
            Thread->user = user; // Передаем структуру в аргументы потока
            usersArr[pthcount] = user; // Добавляем в массив дескриптор
            ++count;
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
