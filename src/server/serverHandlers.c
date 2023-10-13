#include "stdio.h"
#include <unistd.h>
#include "../../include/serverHandlers.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief Производит обмен ключами RSA с пользователями
 * 
 * @param user - стуктура пользователя
 * @param key - стуктура ключей сервера
 * @param fd  - file descriptor пользователя
 * @return - 0 при успешном выполнении, -1 при ошибке
 */
int serverKeyHandler(struct users *user, struct keys *key, int fd) {
    int count = 0;
    long sizeOfOut;
    // Преобразовываем ключ в строку
    BIO *bio_mem = BIO_new(BIO_s_mem());
    PEM_write_bio_PUBKEY(bio_mem, key->pubKey);
    char *pubkey_pem;
    long pubkey_pem_size = BIO_get_mem_data(bio_mem, &pubkey_pem); // длина ключа

    while (1) {
        if (count == 0) {
            if(read(fd, &sizeOfOut, sizeof(long)) <= 0) { // Получаем длину ключа пользователя
                printf("Ошибка получения длины ключа\n");
                return -1;
            }
            write(fd, &pubkey_pem_size, sizeof(long)); // Отправляем пользователю длину ключа сервера
        } else {
            unsigned char *buffer = (unsigned char *) malloc(sizeOfOut + 1); // Получаем ключ пользователя
            if(read(fd, buffer, sizeOfOut) <= 0) {
                printf("Ошибка получения ключа\n");
                return -1;
            }
            buffer[sizeOfOut] = '\0';
            BIO *bio_memory = BIO_new_mem_buf(buffer, -1);
            user->pubKey = PEM_read_bio_PUBKEY(bio_memory, NULL, NULL, NULL); // Записываем ключ в структуру пользователя
            if (user->pubKey == NULL) {
                printf("Ошибка восстановления ключа\n");
                return -1;
            }
            fflush(stdout);
            write(fd, pubkey_pem, pubkey_pem_size); // Отправляем пользователю ключ сервера
            free(buffer);
            BIO_free(bio_mem);
            BIO_free(bio_memory);
            user->msgCount = 1;
            break;
        }
        count++;
    }
    return 0;
}
/**
 * @brief Генерирует приветственное сообщение для пользователя
 *
 * @param username - имя пользователя
 * @return dst
 */
char* WelcomeMsg(char *username) {
    char *helloMsg = "Welcome to chat, ";
    char *dst = (char *) malloc(strlen(username) + strlen(helloMsg) + 2);
    strcpy(dst, helloMsg);
    strcat(dst, username);
    strcat(dst, "!");
    return dst;
}

/**
 * @brief - генерирует сообщение
 *
 * @param buffer - итоговое сообщение
 * @param username  - имя пользователя
 * @param msgBuff - первоначальное сообщение
 */
void MsgBufferHandler(char *buffer, const char *username, const char *msgBuff) {
    strcpy(buffer, username);
    strcat(buffer, ": ");
    strcat(buffer, msgBuff);
}

/**
 *
 * @param usersArr - массив пользователей
 * @return - номер нового пользователя в массиве
 */
int newUser(struct users ** usersArr) {
    int i = 0;
    while (usersArr[i] != NULL) {
        i++;
    }
    return i;
}