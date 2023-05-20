#ifndef C_CHAT_HANDLERS_H
#define C_CHAT_HANDLERS_H
#include "structures.h"

void serverKeyHandler(struct users *user, struct keys *key, int fd);
char *serverMsgHandler(int fd, int *valread, int d, int n);
char* WelcomeMsg(char *username);

#endif
