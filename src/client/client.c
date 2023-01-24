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
#include "locale.h"

#define MSGLEN 2048

WINDOW *chat, *input, *users;

struct keys key;
struct keys serverKeys;

char name[MSGLEN];

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
                wattron(chat,COLOR_PAIR(1));
                printLogMsg(chat, "> ");
                printLogMsg(chat, buffer);
                printLogMsg(chat, "\n");
                wattron(chat, COLOR_PAIR(2));
            } else {
                printLogMsg(chat, "Server error...\n");
                printLogMsg(chat, "Exiting...\n");
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
            wattron(input,count == 2 ? COLOR_PAIR(3) : COLOR_PAIR(2));
            wattron(chat,count == 2 ? COLOR_PAIR(3) : COLOR_PAIR(2));
            char buffer[MSGLEN];
            long encMsg[MSGLEN] = {0};
            size_t encMsgLen = sizeof(encMsg)/(sizeof encMsg[0]);
            if (count <= 2) {

                wattron(chat,COLOR_PAIR(3));
                printLogMsg(chat, "Enter username: ");
                wattron(chat, COLOR_PAIR(2));

                wattron(input, COLOR_PAIR(2));
                printLogMsg(input, "[user]: ");
                wattron(input, COLOR_PAIR(3));
            } else {
                printLogMsg(input, "[");
                printLogMsg(input, name);
                printLogMsg(input, "]: ");
            }
            wgetstr(input, buffer);
            if (!strcmp(buffer, ":q")) {
                exit(0);
            } else if (!strcmp(buffer, ":help")) {
                printHelp(chat, input);
                continue;
            }
            count == 2 ? strcpy(name, buffer) : "";
            encrypt(buffer, encMsg, serverKeys.e, serverKeys.n);
            write(fd, encMsg, encMsgLen);
            strcat(buffer, "\n");
            printLogMsg(chat, buffer);

            wattron(input, COLOR_PAIR(2));
            wattron(chat, COLOR_PAIR(2));

            refresh();
            wclear(input);
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

    setlocale(LC_CTYPE, "");
    initNcurses();

    chat = create_newwin(LINES - 2, COLS, 0, 0);
    scrollok(chat,TRUE);

    wrefresh(chat);

    input = create_newwin(2, COLS, LINES - 2, 0);
    printLogMsg(chat, "Generating keys...\n");
    wrefresh(input);

    generateKeys(&key);

    printLogMsg(chat, "done generating keys\n");

    wattron(chat,COLOR_PAIR(4));
    printLogMsg(chat, "Welcome to C-Chat!\n\t \":help\" to get more information \n");
    wattron(chat, COLOR_PAIR(2));

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
