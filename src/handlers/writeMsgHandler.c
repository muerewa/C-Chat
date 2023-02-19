#include "string.h"
#include "unistd.h"
#include "../../include/msgHandlers.h"
#include "../../include/shifre.h"

void writeMsgHandler(int fd, char *newBuffer, int e, int n) {
    int size = strlen(newBuffer);
    long encMsg[size];
    encrypt(newBuffer, encMsg, size, e, n);
    write(fd , &size, sizeof(int));
    write(fd , encMsg, size * sizeof(long)); // Отправляем сообщение всем кроме нас
}