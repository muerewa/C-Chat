#ifndef C_CHAT_LOG_H
#define C_CHAT_LOG_H

void printUserLogMsg(int fd, char *name, char *msg);
void printServerLogMsg(char *msg, bool removeSignal);

#endif
