#ifndef C_CHAT_MSGHANDLERS_H
#define C_CHAT_MSGHANDLERS_H

char *readMsgHandler(int fd, int *valread, int d, int n);
void writeMsgHandler(int fd, char *newBuffer, int e, int n);

#endif
