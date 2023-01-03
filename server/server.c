#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "unistd.h"
#include "../wrapper/wrappers.h"
#include "stdlib.h"
#include <sys/time.h>
#include "string.h"

int main() {
    int server = Socket(AF_INET, SOCK_STREAM, 0); // Создаем сокет

    struct sockaddr_in addr = {0}; // Создаем адресс сокета
    addr.sin_family = AF_INET; // Семейство протоколов(ipv4)
    addr.sin_port = htons(3030); // Порт

    Bind(server, (const struct sockaddr *) &addr, sizeof addr); // Привязываем к сокету адресс
    Listen(server, 5); // Прослушваем(5 человек максимум могут находиться в очереди)

    socklen_t addrlen = sizeof addr;

    while (1) {
        int fd = Accept(server, (struct sockaddr*) &addr, &addrlen);
        ssize_t nread;
        char buffer[256];
        nread = read(fd, buffer, 256);
        ErrorHandler(nread, "read");
        write(STDOUT_FILENO, buffer, nread);
        write(fd, buffer, nread);
    }

    return 0;
}
