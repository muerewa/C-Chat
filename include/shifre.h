#ifndef C_CHAT_SHIFRE_H
#define C_CHAT_SHIFRE_H

void encrypt(char *msg, long *buffer, int e, int n);
void decrypt(long *encMsg, size_t encMsgSize,char *buffer, int d, int n);

#endif
