#ifndef C_CHAT_STRUCTURES_H
#define C_CHAT_STRUCTURES_H
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

struct keys {
    EVP_PKEY *pubKey;
    EVP_PKEY *privKey;
};

struct users {
    int fd; // файловый дескриптор
    int msgCount; // счетчик сообщений
    char *name; // имя пользователя
    EVP_PKEY *pubKey;
};

struct args {
    int pthcount; // номер пользователя
    int fd; // файловый дескриптор
    struct users *user; // структура пользователя
    struct keys *key;
    struct users **usersArr;
    char **nicknames;
    int *count;
}; // Аргументы для функции Connection

#endif
