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
static void ErrorHandler(char *errorMsg, int *statusCode) {
    printf("%s", errorMsg);
    fflush(stdout);
    *statusCode = -1;
}

char *readMsgHandler(int fd, int *valread, struct keys *key, int *statusCode) {
    size_t size = 0;

    int sread = read(fd, &size, sizeof(size_t));

    char *encryptedData = malloc(size);
    *valread = read(fd, encryptedData, size);

    if (*valread == 0) {
        free(encryptedData);
        return "";
    } else if (*valread < 0) {
        if (sread <= 0) {
            ErrorHandler("Ошибка при получении длины сообщения\n",statusCode);
            free(encryptedData);
            return "";
        }
        free(encryptedData);
        ErrorHandler("Ошибка при получении сообщения\n", statusCode);
        return "";
    }
    // Создание контекста дешифроцирования RSA
    EVP_PKEY_CTX *ctxDecrypt = EVP_PKEY_CTX_new(key->privKey, NULL);
    if (ctxDecrypt == NULL) {
        free(encryptedData);
        ErrorHandler("Ошибка при создании контекста дешифрования RSA\n", statusCode);
        return "";
    }

    // Инициализация операции дешифрования
    if (EVP_PKEY_decrypt_init(ctxDecrypt) != 1) {
        EVP_PKEY_CTX_free(ctxDecrypt);
        free(encryptedData);
        ErrorHandler("Ошибка при инициализации дешифрования RSA\n", statusCode);
        return "";
    }

    // Вычисление размера выходного буфера для расшифрованных данных
    size_t decryptedLen;
    if (EVP_PKEY_decrypt(ctxDecrypt, NULL, &decryptedLen, (unsigned char *)encryptedData, size) != 1) {
        EVP_PKEY_CTX_free(ctxDecrypt);
        free(encryptedData);
        ErrorHandler("Ошибка при вычислении размера расшифрованных данных\n", statusCode);
        return "";
    }

    // Расшифровка данных
    char *decryptedData = (char *)malloc(decryptedLen);
    if (EVP_PKEY_decrypt(ctxDecrypt, (unsigned char *)decryptedData, &decryptedLen, encryptedData, size) != 1) {
        EVP_PKEY_CTX_free(ctxDecrypt);
        free(encryptedData);
        free(decryptedData);
        ErrorHandler("Ошибка при расшифровке данных\n", statusCode);
        return "";
    }
    decryptedData[decryptedLen] = '\0';
    return decryptedData;
}