#include "stdio.h"
#include "stdlib.h"
#include "../../include/RSA.h"
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
/**
 * @brief 
 * 
 * @param key 
 */
void generateKeys(struct keys *key) {
    OpenSSL_add_all_algorithms();
    EVP_PKEY *pkey = EVP_PKEY_new();
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    if (ctx == NULL) {
        printf("Ошибка при создании контекста ключа RSA\n");
        exit(1);
    }
    if (EVP_PKEY_keygen_init(ctx) != 1) {
        printf("Ошибка при инициализации генерации ключей RSA\n");
        EVP_PKEY_CTX_free(ctx);
        exit(1);
    }
    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048) != 1) {
        printf("Ошибка при установке длины ключей RSA\n");
        EVP_PKEY_CTX_free(ctx);
        exit(1);
    }
    if (EVP_PKEY_keygen(ctx, &pkey) != 1) {
        printf("Ошибка при генерации ключей RSA\n");
        EVP_PKEY_CTX_free(ctx);
        exit(1);
    }
    BIO *outBio = BIO_new(BIO_s_mem());
    if (!PEM_write_bio_PrivateKey(outBio, pkey, NULL, NULL, 0, NULL, NULL)) {
        printf("Ошибка при записи приватного ключа в BIO\n");
        EVP_PKEY_CTX_free(ctx);
        BIO_free_all(outBio);
        exit(1);
    }
    if (!PEM_write_bio_PUBKEY(outBio, pkey)) {
        printf("Ошибка при записи публичного ключа в BIO\n");
        EVP_PKEY_CTX_free(ctx);
        BIO_free_all(outBio);
        exit(1);
    }
    EVP_PKEY *privKey = PEM_read_bio_PrivateKey(outBio, NULL, NULL, NULL);
    if (privKey == NULL) {
        printf("Ошибка при чтении приватного ключа из BIO\n");
        EVP_PKEY_CTX_free(ctx);
        BIO_free_all(outBio);
        exit(1);
    }

    EVP_PKEY *pubKey = PEM_read_bio_PUBKEY(outBio, NULL, NULL, NULL);
    if (pubKey == NULL) {
        printf("Ошибка при чтении публичного ключа из BIO\n");
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(privKey);
        BIO_free_all(outBio);
        exit(1);
    }

    fflush(stdout);
    // Освобождение памяти
    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(pkey);
    BIO_free_all(outBio);

    // Очистка ресурсов библиотеки OpenSSL
    EVP_cleanup();
    CRYPTO_cleanup_all_ex_data();

    key->privKey = privKey;
    key->pubKey = pubKey;
}
