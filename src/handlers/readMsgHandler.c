#include "../../include/msgHandlers.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "../../include/shifre.h"

/**
 *
 * @param fd
 * @param valread
 * @param d
 * @param n
 * @return
 */
char *readMsgHandler(int fd, int *valread, int d, int n) {
    int size;
    read(fd, &size, sizeof(int));

    long encMsg[size];
    char *buffer = (char *) malloc(size + 1);
    *valread = read(fd, encMsg, size * sizeof(long));

    if (*valread <= 0) {
        return "";
    }

    decrypt(encMsg, size, buffer, d, n);
    buffer[size] = '\0';

    return buffer;
}