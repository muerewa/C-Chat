#include "stdio.h"
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include "stdbool.h"


/**
 * @brief Получаем текущую время и дату
 * 
 * @param dateTime 
 */
void getTime(char *dateTime) {
    time_t timer;
    struct tm* tm_info;

    timer = time(NULL);
    tm_info = localtime(&timer);

    strftime(dateTime, 26, "[%d-%m-%Y %H:%M:%S]", tm_info);
}


/**
 * @brief Выводит лог пользователя
 * 
 * @param fd - сокет пользователя
 * @param name - имя пользователя
 * @param msg - имя пользователя
 */
void printUserLogMsg(int fd, char *name, char *msg) {
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);

    getpeername(fd, (struct sockaddr *)&addr, &addr_size);

    char time[50] = {0};

    getTime(time);

    if (strcmp(name, "") == 0) {
        name = "\b";
    }

    printf("%s (%s:%d) %s %s\n", time, inet_ntoa(addr.sin_addr), htons(addr.sin_port), name, msg);
    fflush(stdout);
}

/**
 * @brief Выводит лог сервера
 * 
 * @param msg 
 * @param removeSignal 
 */
void printServerLogMsg(char *msg, bool removeSignal) {
    char time[50];
    getTime(time);

    if (removeSignal) {
        printf("\b\b");
    }

    printf("%s %s\n", time, msg);
    fflush(stdout);
}