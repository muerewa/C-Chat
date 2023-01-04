#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "unistd.h"
#include "../wrapper/wrappers.h"
#include <pthread.h>
#include "stdbool.h"
#include "string.h"
#include "stdlib.h"

int users[30];
int count = 0;

struct args {
    int pthcount;
    int fd;
};

void *Connection(void *argv) {
    while (true) {
        char buffer[256];
        int valread;
        int fd = ((struct args*)argv)->fd;
        if((valread = read(fd, buffer, 255)) > 0) {
            buffer[valread] = '\0';
            for (int i = 0; i < count; ++i) {
                if(users[i] != fd) {
                    send(users[i] , buffer , strlen(buffer) , 0 );
                }
            }
        }
    }
}

void ConnLoop(int server, struct sockaddr *addr, socklen_t *addrlen) {

    while (true) {
        if(count <= 30) {
            int fd = Accept(server, addr, addrlen);
            users[count++] = fd;
            char buffer[30];
            sprintf(buffer, "Connection %d", count);
            puts(buffer);
            pthread_t thread_id = count;
            int pthcount = count;
            struct args *Thread = (struct args *)malloc(sizeof(struct args));
            Thread->pthcount = pthcount;
            Thread->fd = fd;
            pthread_create(&thread_id, NULL, Connection, (void *)Thread);
        }
    }
}

int main() {
    int server = Socket(AF_INET, SOCK_STREAM, 0); // Создаем сокет

    struct sockaddr_in addr = {0}; // Создаем адресс сокета
    addr.sin_family = AF_INET; // Семейство протоколов(ipv4)
    addr.sin_port = htons(3030); // Порт

    Bind(server, (const struct sockaddr *) &addr, sizeof addr); // Привязываем к сокету адресс
    Listen(server, 5); // Прослушваем(5 человек максимум могут находиться в очереди)

    socklen_t addrlen = sizeof addr;
    ConnLoop(server, (struct sockaddr*) &addr, &addrlen);

    return 0;
}


