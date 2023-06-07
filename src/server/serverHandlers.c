#include "stdio.h"
#include <unistd.h>
#include "../../include/serverHandlers.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief 
 * 
 * @param user 
 * @param key 
 * @param fd 
 */
void serverKeyHandler(struct users *user, struct keys *key, int fd) {
    while (1) {
        int buffer = 0;

        if(read(fd, &buffer, sizeof(buffer)) != 0) {
            if (user->msgCount == 0) {
                user->e = buffer;
                write(fd, &key->e, sizeof(key->e));
                user->msgCount = 1;
            } else if (user->msgCount == 1) {
                user->n = buffer;
                write(fd, &key->n, sizeof(key->n));
                break;
            }
        }
    }
}

char* WelcomeMsg(char *username) {
    char *dst =  (char *) malloc(strlen(username));
    char *helloMsg = "Welcome to chat, ";
    dst = realloc(dst, strlen(helloMsg) + 2);
    strcat(dst, helloMsg);
    strcat(dst, username);
    strcat(dst, "!");
    return dst;
}

void MsgBufferHandler(char *buffer, const char *username, const char *msgBuff) {
    strcpy(buffer, username);
    strcat(buffer, ": ");
    strcat(buffer, msgBuff); // Добавляем в буффер сообщение
}