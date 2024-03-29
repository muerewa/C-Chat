#ifndef C_CHAT_HANDLERS_H
#define C_CHAT_HANDLERS_H
#include "structures.h"

int serverKeyHandler(struct users *user, struct keys *key, int fd);
char *serverMsgHandler(int fd, int *valread, int d, int n);
char* WelcomeMsg(char *username);
void MsgBufferHandler(char *buffer, const char *username, const char *msgBuff);
int newUser(struct users ** usersArr);

#endif
