#include <stdio.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

#include "../../include/RSA.h"

/**
 * @brief - обрабатываем ошибки
 *
 * @param buffer - сообщение об ошибке
 */
static void Error(char *buffer) {
    printf("%s \n", buffer);
    exit(1);
}

/**
 * @brief - генерирует приватный и публичный ключи RSA
 * 
 * @param key - структура ключей RSA
 */
void generateKeys(struct keys *key) {
    OpenSSL_add_all_algorithms(); // Инициализируем библиотеку openssl

    EVP_PKEY *pkey = EVP_PKEY_new();
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL); // Создаем контекст ключа

    if (ctx == NULL) { Error("Ошибка при создании контекста ключа RSA\n"); }

    if (EVP_PKEY_keygen_init(ctx) != 1) { printf("Ошибка при инициализации генерации ключей RSA\n"); } // Инициализируем ключи

    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048) != 1) { Error("Ошибка при установке длины ключей RSA\n"); } // Устанавливаем длину ключей

    if (EVP_PKEY_keygen(ctx, &pkey) != 1) { Error("Ошибка при генерации ключей RSA\n");} // Генерируем ключи

    // Записываем приватный ключ
    BIO *outBio = BIO_new(BIO_s_mem());
    if (!PEM_write_bio_PrivateKey(outBio, pkey, NULL, NULL, 0, NULL, NULL)) { Error("Ошибка при записи приватного ключа в BIO\n");}

    if (!PEM_write_bio_PUBKEY(outBio, pkey)) { Error("Ошибка при записи публичного ключа в BIO\n");}

    // Читаем приватный ключ
    EVP_PKEY *privKey = PEM_read_bio_PrivateKey(outBio, NULL, NULL, NULL);
    if (privKey == NULL) { Error("Ошибка при чтении приватного ключа из BIO\n");}

    // Читаем публичный ключ
    EVP_PKEY *pubKey = PEM_read_bio_PUBKEY(outBio, NULL, NULL, NULL);
    if (pubKey == NULL) { Error("Ошибка при чтении публичного ключа из BIO\n"); }

    fflush(stdout);
    // Освобождение памяти
    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(pkey);
    BIO_free_all(outBio);

    // Очистка ресурсов библиотеки OpenSSL
    EVP_cleanup();
    CRYPTO_cleanup_all_ex_data();

    // Сохраняем ключи в структуру
    key->privKey = privKey;
    key->pubKey = pubKey;
}
