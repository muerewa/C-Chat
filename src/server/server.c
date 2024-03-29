#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include "../../include/connection.h"
#include "../../include/RSA.h"
#include "../../include/wrappers.h"
#include "../../include/log.h"
#include "../../include/serverHandlers.h"
#include "../../include/msgHandlers.h"

#define MAXUSERS 30 // Максимальное кол-во пользователей + 1

struct users* usersArr[30] = {NULL}; // Массив пользователей публичной комнаты
char *nicknames[30] = {NULL}; // Массив имен пользователей публичной комнаты

struct users* privateUsersArr[30] = {NULL}; // Массив пользователей приватной комнаты
char *privateNicknames[30] = {NULL}; // Массив имен пользователей приватной комнаты

char *password; // Пароль для приватной комнаты

int count = 0; // Счетчик пользователей публичной комнаты
int privateCount = 0; // Счетчик пользователей приватной комнаты
struct keys key; // Струтура RSA ключей сервера

int *serverSocket = NULL; // Указатель на сокет сервера
/**
 * @brief - обработчик остановки сервера
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

/**
 * @brief - функция для распределения в комнаты
 *
 * @param roomType - тип комнаты, полученный от пользователя
 * @param Thread - структура аргументов, передаваемые в фукнцию Connection
 * @param pthcount - номер пользователя в массиве пользователей
 * @param user - структура пользователя
 * @param fd - file descriptor пользователя
 *
 * @return - 0 при успешном выполнении, -1 при ошибке, 1 при нехватке мест
 */
int roomHandler(char *roomType, struct args *Thread, int *pthcount, struct users *user, int fd) {
    if (strcmp(roomType,"public") == 0) {
        if (count > MAXUSERS) {
            return 1;
        }
        Thread->usersArr = usersArr;
        Thread->nicknames = nicknames;
        Thread->count = &count;
        *pthcount = newUser(usersArr);
        usersArr[*pthcount] = user;
        ++count;
        return 0;
    } else if (strcmp(roomType,"private") == 0) {
        if (privateCount > MAXUSERS) {
            return 1;
        }
        int statcode = 0, valread;
        char *pass= readMsgHandler(fd, &valread, &key, &statcode);
        if (strcmp(pass, password) == 0) {
            Thread->usersArr = privateUsersArr;
            Thread->nicknames = privateNicknames;
            Thread->count = &privateCount;
            *pthcount = newUser(privateUsersArr);
            privateUsersArr[*pthcount] = user;
            ++privateCount;
            free(pass);
        } else {
            free(pass);
            return -1;
        }
        return 0;
    }
    return -1;
}

/**
 * @brief Функция, обрабатывающая новых пользователей
 * 
 * @param server - file descriptor сервера
 * @param addr - адрес сокета
 * @param addrlen - размер адреса
 */
void ConnLoop(int server, struct sockaddr *addr, socklen_t *addrlen) {
    while (true) {
        if(count <= MAXUSERS || privateCount <= MAXUSERS) {
            int fd = Accept(server, addr, addrlen); // Принимаем новое подключение
            pthread_t thread_id = count; // создаем id потока

            printUserLogMsg(fd, "", "connected"); // Вывод лога о присоеденении сокета

            struct args *Thread = (struct args *)malloc(sizeof(struct args)); // Инициализируем структуру аргументов

            Thread->fd = fd;
            Thread->key = &key;

            struct users *user = malloc(sizeof(struct users)); // Инициализируем структуру
            user->fd = fd;
            user->msgCount = 0;

            if (serverKeyHandler(user,&key,fd) == -1) { // Обрабатываем ошибку при неуспешном выполненнии serverKeyHandler
                close(fd); // Закрываем сокет пользователя
                free(user); // Освобождаем структуру пользователя
                free(Thread); // Освобождаем структуру аргументов
                continue;
            }

            int statcode = 0, valread, pthcount = 0;
            char *roomType = readMsgHandler(fd, &valread, &key, &statcode); // Получаем тип комнаты от пользователя

            int res = roomHandler(roomType, Thread, &pthcount, user, fd);

            free(roomType);
            if (res != 0) {
                if(res == -1) { // Обработка ошибки при неуспешном выполнении roomHandler
                    writeMsgHandler(user->fd,"Некорректное сообщение или неправильный пароль", user->pubKey); // Отправляем пользователю сообщение о неудаче
                } else if (res == 1) {
                    writeMsgHandler(user->fd,"Прывшено максимальное кол-во пользователей. Попробуйте присоедениться позже", user->pubKey); // Отправляем пользователю сообщение о неудаче
                }
                printUserLogMsg(fd, "", "disconnected");
                close(fd); // Закрываем сокет пользователя
                EVP_PKEY_free(user->pubKey); // Освобождаем ключ пользователя
                free(user); // Освобождаем структуру пользователя
                free(Thread); // Освобождаем структуру аргументов
                continue;
            }
            Thread->pthcount = pthcount;
            user->msgCount = 0;
            Thread->user = user; // Передаем структуру в аргументы потока

            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); // Создаем и инициализируем аргументы для потока
            pthread_create(&thread_id, &attr, Connection, (void *)Thread); // Создаем отдельный поток для каждого пользователя
        }
    }
}

 
/**
 * @brief main функция серверной части чата
 * 
 * @return int 
 */
int main(int argc, char **argv) {

    char *ip;
    int port;
    int rez;
    // Обрабатываем аргументы командной строки
    while ( (rez = getopt(argc, argv, "hi:p:s:")) != -1){
        switch (rez) {
            case 'h': printf("Use -i to set ip of server, -p to set port, -s to set secret phrase for private room\n"); break;
            case 'i': ip = optarg; break;
            case 'p': port = atoi(optarg); break;
            case 's': password = optarg; break;
        }
    }

    if (port == 0 || ip == NULL || password == NULL) {
        printf("Invalid arguments. Try -h to get help\n");
        exit(0);
    }

    printf("Generating keys...\n");
    fflush(stdout);

    generateKeys(&key); // Генерируем ключи RSA

    printf("done generating keys\n");
    fflush(stdout);

    printf("starting server...\n");
    fflush(stdout);

    signal(SIGINT, intHandler);

    int server = Socket(AF_INET, SOCK_STREAM, 0); // Создаем сокет
    serverSocket = &server;
    struct sockaddr_in addr = {0}; // Создаем адресс сокета
    Inet_ptonfd(AF_INET, ip, &addr.sin_addr); // Устанавливаем ip сервера
    addr.sin_family = AF_INET; // Семейство протоколов(ipv4)
    addr.sin_port = htons(port); // Порт


    Bind(server, (const struct sockaddr *) &addr, sizeof addr); // Привязываем к сокету адресс
    Listen(server, 5); // Прослушваем(5 человек максимум могут находиться в очереди)

    socklen_t addrlen = sizeof addr; // Размер адреса

    printf("server running\n");
    fflush(stdout);

    ConnLoop(server, (struct sockaddr*) &addr, &addrlen); // Запускаем принятие пользователей
    return 0;
}
