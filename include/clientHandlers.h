#ifndef C_CHAT_CLIENTHANDLERS_H
#define C_CHAT_CLIENTHANDLERS_H

int commandHandler(char *buffer, char *MAGENTA, char *RESET);
char *clientMsgHandler(int fd, int *valread, int d, int n);

#endif
