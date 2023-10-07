#include "string.h"
#include "unistd.h"
#include "../../include/msgHandlers.h"
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

#define MAX_CIPHER_SIZE 2048

static void ErrorHandler(EVP_PKEY *pubKey, char *errorMsg) {
    printf("%s", errorMsg);
    EVP_PKEY_free(pubKey);
    exit(0);
}

/**
 *
 * @param fd
 * @param newBuffer
 * @param e
 * @param n
 */
void writeMsgHandler(int fd, char *newBuffer, EVP_PKEY *pubKey) {
    int size = strlen(newBuffer);

    EVP_PKEY_CTX *ctxEncrypt = EVP_PKEY_CTX_new(pubKey, NULL);
    if (ctxEncrypt == NULL) {
        ErrorHandler(pubKey, "Ошибка при создании контекста шифрования RSA\n");
        exit(0);
    }

    // Инициализация операции шифрования
    if (EVP_PKEY_encrypt_init(ctxEncrypt) != 1) {
        ErrorHandler(pubKey, "Ошибка при инициализации шифрования RSA\n");
        exit(0);
    }

    // Вычисление размера выходного буфера для шифротекста
    size_t encryptedLen;
    if (EVP_PKEY_encrypt(ctxEncrypt, NULL, &encryptedLen, (const unsigned char *)newBuffer, size) != 1) {
        EVP_PKEY_CTX_free(ctxEncrypt);
        ErrorHandler(pubKey, "Ошибка при вычислении размера шифротекста\n");
        exit(0);
    }

    // Шифрование данных
    unsigned char *encryptedData = (unsigned char *)malloc(encryptedLen);
    if (EVP_PKEY_encrypt(ctxEncrypt, encryptedData, &encryptedLen, (const unsigned char *)newBuffer, size) != 1) {
        EVP_PKEY_CTX_free(ctxEncrypt);
        free(encryptedData);
        ErrorHandler(pubKey, "Ошибка при шифровании данных\n");
        exit(0);
    }
    write(fd , &encryptedLen, sizeof(size_t));
    write(fd , encryptedData, encryptedLen);
}