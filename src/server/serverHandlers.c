#include "stdio.h"
#include <unistd.h>
#include "../../include/serverHandlers.h"
#include "../../include/structures.h"
#include "stdlib.h"
#include "stdio.h"
#include "../../include/shifre.h"
#include "string.h"

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

char *serverMsgHandler(int fd, int *valread, int d, int n) {
    int size;
    read(fd, &size, sizeof(int));

    long encMsg[size];
    char *buffer = (char *) malloc(size + 1);
    *valread = read(fd, encMsg, size * sizeof(long));
    decrypt(encMsg, size, buffer, d, n);
    buffer[size] = '\0';

    return buffer;
}