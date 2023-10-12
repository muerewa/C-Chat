#include "../../include/connection.h"
#include <stdio.h>
#include <pthread.h>
#include "stdbool.h"
#include "string.h"
#include "stdlib.h"
#include <openssl/evp.h>

#include "../../include/structures.h"
#include "../../include/log.h"
#include "../../include/msgHandlers.h"
#include "../../include/serverHandlers.h"

pthread_mutex_t mutex;

/**
 * @brief
 *
 * @param argv
 * @return void*
 */
void *Connection(void *argv) {

    int fd = ((struct args*)argv)->fd; // Достаем файловый дескриптор из аргументов

    int pthcount = ((struct args*)argv)->pthcount; // Достаем номер пользователя
    struct users *user = ((struct args*)argv)->user; // Достаем структуру юзера

    struct keys *key = ((struct args*)argv)->key;

    while (true) {

        struct users **usersArr = ((struct args*)argv)->usersArr;
        char **nicknames = ((struct args*)argv)->nicknames;
        int *count = ((struct args*)argv)->count;

        int statcode = 0;
        int valread;
        char *buffer = readMsgHandler(fd, &valread, key, &statcode);

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

            for (int i = 0; i < *count; ++i) { // Проходимся по массиву сокетов
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

            if ((user->msgCount != 0) && *count >= 1) {
                for (int i = 0; i < *count; ++i) { // Проходимся по массиву сокетов
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
            EVP_PKEY_free(user->pubKey);
            free(user); // Освобождаем структуру
            free(disbuffer);
            free(argv);
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL); // Выходим из потока
        }
    }
}
