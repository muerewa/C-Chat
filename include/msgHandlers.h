#ifndef C_CHAT_MSGHANDLERS_H
#define C_CHAT_MSGHANDLERS_H
#include "structures.h"

char *readMsgHandler(int fd, int *valread, struct keys *key);
void writeMsgHandler(int fd, char *newBuffer, EVP_PKEY *pubKey);

#endif
