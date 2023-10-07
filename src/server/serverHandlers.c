#include "stdio.h"
#include <unistd.h>
#include "../../include/serverHandlers.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief 
 * 
 * @param user 
 * @param key 
 * @param fd 
 */
void serverKeyHandler(struct users *user, struct keys *key, int fd) {
    int count = 0;
    long sizeOfOut;
    BIO *bio_mem = BIO_new(BIO_s_mem());
    PEM_write_bio_PUBKEY(bio_mem, key->pubKey);
    char *pubkey_pem;
    long pubkey_pem_size = BIO_get_mem_data(bio_mem, &pubkey_pem);

    while (1) {
        if (count == 0) {
            if(read(fd, &sizeOfOut, sizeof(long)) == 0) {
                printf("Ошибка получения длины ключа");
                exit(0);
            }
            write(fd, &pubkey_pem_size, sizeof(long));
        } else {
            unsigned char *buffer = malloc(sizeOfOut);
            if(read(fd, buffer, sizeOfOut) == 0) {
                printf("Ошибка получения ключа");
                exit(0);
            }
            BIO *bio_memory = BIO_new_mem_buf(buffer, -1);
            user->pubKey = PEM_read_bio_PUBKEY(bio_memory, NULL, NULL, NULL);
            if (user->pubKey == NULL) {
                printf("Ошибка восстановления ключа\n");
                exit(0);
            }
            write(fd, pubkey_pem, pubkey_pem_size);
            free(buffer);
            BIO_free(bio_mem);
            BIO_free(bio_memory);
            user->msgCount = 1;
            break;
        }
        count++;
    }
}

char* WelcomeMsg(char *username) {
    char *helloMsg = "Welcome to chat, ";
    char *dst = (char *) malloc(strlen(username) + strlen(helloMsg) + 2);
    strcpy(dst, helloMsg);
    strcat(dst, username);
    strcat(dst, "!");
    return dst;
}

void MsgBufferHandler(char *buffer, const char *username, const char *msgBuff) {
    strcpy(buffer, username);
    strcat(buffer, ": ");
    strcat(buffer, msgBuff); // Добавляем в буффер сообщение
}