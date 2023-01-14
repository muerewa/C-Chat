#include "stdio.h"
#include <arpa/inet.h>
#include <time.h>
#include "stdbool.h"

void getTime(char *dateTime) { // Получаем текущую время и дату
    time_t timer;
    struct tm* tm_info;

    timer = time(NULL);
    tm_info = localtime(&timer);

    strftime(dateTime, 26, "[%d-%m-%Y %H:%M:%S]", tm_info);
}

void printUserLogMsg(int fd, char *name, char *msg) { // Получаем ip и port пользователя
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    getpeername(fd, (struct sockaddr *)&addr, &addr_size);

    char time[50];
    getTime(time);

    if (name == "") {
        name = "\b";
    }

    printf("%s (%s:%d) %s %s\n", time, inet_ntoa(addr.sin_addr), htons(addr.sin_port), name, msg);
    fflush(stdout);
}

void printServerLogMsg(char *msg, bool removeSignal) {
    char time[50];
    getTime(time);

    if (removeSignal) {
        printf("\b\b");
    }

    printf("%s %s\n", time, msg);
    fflush(stdout);
}