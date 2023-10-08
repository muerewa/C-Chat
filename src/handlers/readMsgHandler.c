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
static void ErrorHandler(char *errorMsg) {
    printf("%s", errorMsg);
}

char *readMsgHandler(int fd, int *valread, struct keys *key, int *statusCode) {
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
        ErrorHandler("Ошибка при создании контекста дешифрования RSA\n");
        *statusCode = -1;
        return "";
    }

    // Инициализация операции дешифрования
    if (EVP_PKEY_decrypt_init(ctxDecrypt) != 1) {
        EVP_PKEY_CTX_free(ctxDecrypt);
        free(encryptedData);
        ErrorHandler("Ошибка при инициализации дешифрования RSA\n");
        *statusCode = -1;
        return "";
    }

    // Вычисление размера выходного буфера для расшифрованных данных
    size_t decryptedLen;
    if (EVP_PKEY_decrypt(ctxDecrypt, NULL, &decryptedLen, (unsigned char *)encryptedData, size) != 1) {
        EVP_PKEY_CTX_free(ctxDecrypt);
        free(encryptedData);
        ErrorHandler("Ошибка при вычислении размера расшифрованных данных\n");
        *statusCode = -1;
        return "";
    }

    // Расшифровка данных
    char *decryptedData = (char *)malloc(decryptedLen);
    if (EVP_PKEY_decrypt(ctxDecrypt, (unsigned char *)decryptedData, &decryptedLen, encryptedData, size) != 1) {
        EVP_PKEY_CTX_free(ctxDecrypt);
        free(encryptedData);
        free(decryptedData);
        ErrorHandler("Ошибка при расшифровке данных\n");
        *statusCode = -1;
        return "";
    }
    decryptedData[decryptedLen] = '\0';
    return decryptedData;
}