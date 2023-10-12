#include "stdio.h"
#include "../../include/wrappers.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include "unistd.h"
#include "string.h"
#include "pthread.h"
#include "stdlib.h"
#include "../../include/structures.h"
#include "../../include/RSA.h"
#include "../../include/clientHandlers.h"
#include "../../include/msgHandlers.h"
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

#define MSGLEN 2048

#define GREEN "\033[0;32m"
#define MAGENTA "\033[0;35m"
#define RESET "\033[0m"
#define RED "\033[0;31m"

struct keys key;
struct keys serverKeys;

struct clientArgs {
    int fd;
};


/**
 * @brief 
 * 
 * @param arguments 
 * @return void* 
 */
void *readMsg(void *arguments) {
    int fd = ((struct clientArgs*)arguments)->fd;
    int count = 0;
    long size;
    while (1) {
        if (count == 0) {
            read(fd, &size, sizeof(long));
        }else if (count == 1) {
            char *buffer = malloc(size);
            if (read(fd, buffer, size) == 0) {
                printf("Ошибка получения ключа");
            }
            BIO *bio_memory = BIO_new_mem_buf(buffer, -1);
            serverKeys.pubKey = PEM_read_bio_PUBKEY(bio_memory, NULL, NULL, NULL);
            if (serverKeys.pubKey == NULL) {
                printf("Ошибка восстановления ключа\n");
                exit(0);
            }
            free(bio_memory);
            free(buffer);
        } else {
            int valread;
            int statCode = 0;
            char *buffer = readMsgHandler(fd, &valread, &key, &statCode);
            if (statCode == -1) {
                printf("%s", RED);
                printf("> Server: ошибка при передаче сообщения\n");
                printf("%s", RESET);
                fflush(stdout);
                continue;
            }
            if (valread != 0) {
                printf("%s", GREEN);
                printf("> %s\n", buffer);
                printf("%s", RESET);
                fflush(stdout);
            } else {
                perror("server error\n");
                exit(0);
            }
            free(buffer);
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
    int fd = ((struct clientArgs*)arguments)->fd;
    int count = 0;
    BIO *bio_mem = BIO_new(BIO_s_mem());
    PEM_write_bio_PUBKEY(bio_mem, key.pubKey);
    char *pubkey_pem;
    long size = BIO_get_mem_data(bio_mem, &pubkey_pem);
    free(bio_mem);
    while (1) {
        if (count == 0) {
            write(fd, &size, sizeof(long));
        } else if (count == 1) {
            write(fd, pubkey_pem, size);
        } else {
            char buffer[MSGLEN];
            if (count > 2) {
                printf("%s",RESET);
            }
            fgets(buffer, MSGLEN, stdin);
            buffer[strlen(buffer) - 1] = '\0';
            if (commandHandler(buffer, MAGENTA, RESET)) {
                if (count == 2) {
                    printf("Enter username: ");
                    fflush(stdout);
                }
                continue;
            }
            writeMsgHandler(fd, buffer, serverKeys.pubKey);
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
int main(int argc, char *argv[]) {

    char *ip;
    int port;
    int rez;
    while ( (rez = getopt(argc, argv, "hi:p:")) != -1){
        switch (rez) {
            case 'h': printf("Use -i to set ip of server and -p to set port\n"); break;
            case 'i': ip = optarg; break;
            case 'p': port = atoi(optarg); break;
        }
    }
    if (port == 0 || ip == NULL) {
        printf("Invalid arguments. Try -h to get help\n");
        exit(0);
    }

    printf("Generating keys...\n");
    fflush(stdout);

    generateKeys(&key);

    printf("done generating keys\n");
    fflush(stdout);

    printf("%s", MAGENTA);
    printf("Welcome to C-Chat client!\n\tEnter :help to get help\n\nEnter username: ");
    fflush(stdout);


    int client = Socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};

    addr.sin_family = AF_INET; // Семейство протоколов(ipv4)
    addr.sin_port = htons(port); // Порт сервера
    Inet_ptonfd(AF_INET, ip, &addr.sin_addr); // Задаем айпи сервера

    struct clientArgs *arguments = (struct clientArgs *)malloc(sizeof(struct clientArgs));
    arguments->fd = client;

    Connectfd(client, (struct sockaddr *)&addr, sizeof addr);

    pthread_t thread_id = 0;
    pthread_create(&thread_id, NULL, writeMsg, (void *)arguments);
    pthread_create(&thread_id, NULL, readMsg, (void *)arguments);
    pthread_join(thread_id, NULL);

    return 0;
}
