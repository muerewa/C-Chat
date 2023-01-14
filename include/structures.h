#ifndef C_CHAT_STRUCTURES_H
#define C_CHAT_STRUCTURES_H

struct keys {
    int e;
    int d;
    int n;
};

struct users {
    int fd; // файловый дескриптор
    int msgCount; // счетчик сообщений
    char *name; // имя пользователя
    int e;
    int n;
};

#endif
