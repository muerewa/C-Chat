#ifndef C_CHAT_STRUCTURES_H
#define C_CHAT_STRUCTURES_H
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

struct keys {
    EVP_PKEY *pubKey; // публичный RSA ключ
    EVP_PKEY *privKey; // приватный RSA ключ
};

struct users {
    int fd; // файловый дескриптор
    int msgCount; // счетчик сообщений
    char *name; // имя пользователя
    EVP_PKEY *pubKey; // публичный ключ пользователя
};

struct args {
    int pthcount; // номер пользователя
    int fd; // файловый дескриптор
    struct users *user; // структура пользователя
    struct keys *key; // структура ключей
    struct users **usersArr; // массив пользователей
    char **nicknames; // массив имен пользователей
    int *count; // кол-во пользователей в комнате
}; // Аргументы для функции Connection

#endif
