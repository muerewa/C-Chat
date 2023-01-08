#include "stdio.h"
#include "../../include/wrappers.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include "unistd.h"
#include "string.h"
#include "sys/types.h"
#include "pthread.h"
#include "stdlib.h"
#include "string.h"

struct args {
    int fd;
};

void *readMsg(void *arguments) {
    int fd = ((struct args*)arguments)->fd;
    while (1) {
        char buffer[256] = {};
        int valread = read(fd, buffer, 255);
        buffer[strlen(buffer) - 1] = '\0';
        if(valread != 0) {
            printf("%s\n", buffer);
        } else {
            printf("server error\n");
        }
    }
}

void *writeMsg(void *arguments) {
    int fd = ((struct args*)arguments)->fd;

    while (1) {
        char buffer[256] = {0};
        fgets(buffer, 255, stdin);
        write(fd, buffer, strlen(buffer));
    }
}

int main() {
    int client = Socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};

    addr.sin_family = AF_INET; // Семейство протоколов(ipv4)
    addr.sin_port = htons(3030); // Порт сервера
    Inet_ptonfd(AF_INET, "127.0.0.1", &addr.sin_addr); // Задаем айпи сервера

    struct args *arguments = (struct args *)malloc(sizeof(struct args));
    arguments->fd = client;

    Connectfd(client, (struct sockaddr *)&addr, sizeof addr);

    pthread_t thread_id = 0;
    pthread_create(&thread_id, NULL, writeMsg, (void *)arguments);
    pthread_create(&thread_id, NULL, readMsg, (void *)arguments);
    pthread_join(thread_id, NULL);
    return 0;
}
