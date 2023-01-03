#include "sys/types.h"
#include "sys/socket.h"
#include "../wrapper/wrappers.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "stdio.h"
#include "unistd.h"

int main() {
    int fd = Socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3030);
    Inet_ptonfd(AF_INET, "127.0.0.1", &addr.sin_addr);
    Connectfd(fd, (struct sockaddr*) &addr, sizeof addr);
    write(fd, "Hello\n", 6);
    char buffer[256];
    ssize_t nread;
    nread = read(fd, buffer, 256);
    ErrorHandler(nread, "read failure");
    if (nread == 0) {
        printf("EOF occures\n");
    }
    write(STDOUT_FILENO, buffer, nread);
    close(fd);
    return 0;
}