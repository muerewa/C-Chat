#ifndef C_CHAT_SHIFRE_H
#define C_CHAT_SHIFRE_H

void encrypt(char *msg, int *buffer, int e, int n);
void decrypt(int *encMsg, size_t encMsgSize,char *buffer, int d, int n);


#endif
