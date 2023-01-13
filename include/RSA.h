#ifndef C_CHAT_RSA_H
#define C_CHAT_RSA_H

struct keys {
    int e;
    int d;
    int n;
};

void generateKeys(struct keys *key);


#endif
