#include "stdio.h"
#include "../../include/wrappers.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include "unistd.h"
#include "string.h"
#include "sys/types.h"
#include "pthread.h"
#include "stdlib.h"
#include "../../include/structures.h"
#include "../../include/RSA.h"
#include "../../include/shifre.h"
#include "../../include/graphics.h"
#include "ncurses.h"

#define MSGLEN 2048


struct keys key;
struct keys serverKeys;

struct args {
    int fd;
};


/**
 * @brief
 *
 * @param arguments
 * @return void*
 */
void *readMsg(void *arguments) {
    int fd = ((struct args*)arguments)->fd;
    int count = 0;
    while (1) {
        if (count == 0) {
            int buffer;
            if (read(fd, &buffer, sizeof(buffer)) != 0) {
                serverKeys.e = buffer;
            }
        } else if (count == 1) {
            int buffer;
            if (read(fd, &buffer, sizeof(buffer)) != 0) {
                serverKeys.n = buffer;
            }
        } else {
            char buffer[MSGLEN] = {};
            long encMsg[MSGLEN];
            size_t encMsgLen = sizeof(encMsg)/sizeof(encMsg[0]);
            int valread = read(fd, encMsg, encMsgLen);
            decrypt(encMsg, encMsgLen, buffer,key.d, key.n);
            buffer[strlen(buffer) - 1] = '\0';
            if (valread != 0) {
                attron(COLOR_PAIR(1));
                printLogMsg("> ");
                printLogMsg(buffer);
                printLogMsg("\n");
                attron(COLOR_PAIR(2));
            } else {
                printLogMsg("Server error...\n");
                printLogMsg("Exiting...\n");
                exit(0);
            }
        }
        count++;
    }
}


/**
 * @brief
 *
 * @param arguments
 * @return void*
 */
void *writeMsg(void *arguments) {
    int fd = ((struct args*)arguments)->fd;
    int count = 0;

    while (1) {
        if (count == 0) {
            write(fd, &key.e, sizeof(key.e));
        } else if (count == 1) {
            write(fd, &key.n, sizeof(key.n));
        } else {
            attron(count == 2 ? attron(COLOR_PAIR(3)) : attron(COLOR_PAIR(2)));
            char buffer[MSGLEN];
            long encMsg[MSGLEN] = {0};
            size_t encMsgLen = sizeof(encMsg)/(sizeof encMsg[0]);
            getnstr(buffer, MSGLEN);
            encrypt(buffer, encMsg, serverKeys.e, serverKeys.n);
            write(fd, encMsg, encMsgLen);
            attron(COLOR_PAIR(2));
        }
        count++;
    }
}


/**
 * @brief
 *
 * @param argc
 * @param argv
 * @return int
 */
int main(int argc, char **argv) {

    initscr();
    start_color();
    use_default_colors();

    init_pair(1, COLOR_GREEN, -1); // Зеленый цвет
    init_pair(2, -1, -1); // Дефолтный цвет
    init_pair(3, COLOR_MAGENTA, -1); // Розовый цвет

    printLogMsg("Generating keys...\n");

    generateKeys(&key);

    printLogMsg("done generating keys\n");

    attron(COLOR_PAIR(3));
    printLogMsg("Enter username: ");
    attron(COLOR_PAIR(2));

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

    endwin();

    return 0;
}
