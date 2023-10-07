#include "../../include/msgHandlers.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

#define MAX_CIPHER_SIZE 2048

/**
 *
 * @param fd
 * @param valread
 * @param d
 * @param n
 * @return
 */
static void ErrorHandler(struct keys *key, char *errorMsg) {
    printf("%s", errorMsg);
    EVP_PKEY_free(key->pubKey);
    EVP_PKEY_free(key->privKey);
}

char *readMsgHandler(int fd, int *valread, struct keys *key) {
    size_t size = 0;
    read(fd, &size, sizeof(size_t));

    char *encryptedData = malloc(size);
    *valread = read(fd, encryptedData, size);

    if (*valread <= 0) {
        return "";
    }
    // Создание контекста дешифроцирования RSA
    EVP_PKEY_CTX *ctxDecrypt = EVP_PKEY_CTX_new(key->privKey, NULL);
    if (ctxDecrypt == NULL) {
        free(encryptedData);
        ErrorHandler(key,"Ошибка при создании контекста дешифрования RSA\n");
        exit(1);
    }

    // Инициализация операции дешифрования
    if (EVP_PKEY_decrypt_init(ctxDecrypt) != 1) {
        EVP_PKEY_CTX_free(ctxDecrypt);
        free(encryptedData);
        ErrorHandler(key,"Ошибка при инициализации дешифрования RSA\n");
        exit(0);
    }

    // Вычисление размера выходного буфера для расшифрованных данных
    size_t decryptedLen;
    if (EVP_PKEY_decrypt(ctxDecrypt, NULL, &decryptedLen, (unsigned char *)encryptedData, size) != 1) {
        EVP_PKEY_CTX_free(ctxDecrypt);
        free(encryptedData);
        ErrorHandler(key,"Ошибка при вычислении размера расшифрованных данных\n");
        exit(0);
    }

    // Расшифровка данных
    char *decryptedData = (char *)malloc(decryptedLen);
    if (EVP_PKEY_decrypt(ctxDecrypt, (unsigned char *)decryptedData, &decryptedLen, encryptedData, size) != 1) {
        EVP_PKEY_CTX_free(ctxDecrypt);
        free(encryptedData);
        free(decryptedData);
        ErrorHandler(key,"Ошибка при расшифровке данных\n");
        exit(0);
    }
    decryptedData[decryptedLen] = '\0';
    return decryptedData;
}