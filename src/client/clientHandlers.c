#include "../../include/clientHandlers.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "../../include/shifre.h"

int commandHandler(char *buffer, char *MAGENTA, char *RESET) {
    if (!strcmp(buffer, ":help\n")) {
        printf("%s", MAGENTA);
        printf("\t:help to get help\n");
        printf("%s", RESET);
        fflush(stdout);
        return 1;
    } else if (!strcmp(buffer, ":q\n")) {
        exit(0);
    }
    return 0;
}

char *clientMsgHandler(int fd, int *valread, int d, int n) {
    long size;
    read(fd, &size, sizeof(long));

    long encMsg[size];
    char *buffer = (char *) malloc(size + 1);
    *valread = read(fd, encMsg, size * sizeof(long));
    decrypt(encMsg, size, buffer, d, n);
    buffer[size] = '\0';

    return buffer;
}