#ifndef C_CHAT_STRUCTURES_H
#define C_CHAT_STRUCTURES_H

struct keys {
    int exp;
    int deshifre;
    int mod;
};

struct users {
    int fd; // файловый дескриптор
    int msgCount; // счетчик сообщений
    char *name; // имя пользователя
    int exp;
    int mod;
};

#endif
