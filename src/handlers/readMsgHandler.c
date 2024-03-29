#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <openssl/evp.h>

#include "../../include/msgHandlers.h"

#define MAX_CIPHER_SIZE 2048

/**
 * @brief Обработка ошибок дешифрования
 *
 * @param errorMsg - сообщение об ошибке
 * @param statusCode - код ошибки
 */
static void ErrorHandler(char *errorMsg, int *statusCode) {
    printf("%s", errorMsg);
    fflush(stdout);
    *statusCode = -1;
}

/**
 * @brief Обработчик получения сообщения
 *
 * @param fd - file decriptor
 * @param valread - number read
 * @param key - структура ключей RSA
 * @param statusCode - код ошибки
 * @return
 */
char *readMsgHandler(int fd, int *valread, struct keys *key, int *statusCode) {
    size_t size = 0;

    int sread = read(fd, &size, sizeof(size_t)); // Читаем длину сообщения

    char *encryptedData = malloc(size);
    *valread = read(fd, encryptedData, size); // Читаем зашифрованное сообщение

    // Обрабатываем неккоректные данные
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
    free(encryptedData); // Освобождаем буфер
    EVP_PKEY_CTX_free(ctxDecrypt); // Освобождаем контекст дешифрования
    decryptedData[decryptedLen] = '\0';
    return decryptedData;
}