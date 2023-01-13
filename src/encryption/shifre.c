#include "stdio.h"
#include "string.h"
#include "../../include/shifre.h"

long shifre(long b, long exp, long mod) {
    long ret = 1;

    if (mod == 1)
        return 0;
    b = b % mod;
    while (exp > 0)
    {
        if (exp % 2 == 1)
            ret = (ret * b) % mod;
        exp = exp >> 1;
        b = (b * b) % mod;
    }
    return ret;
}

void encrypt(char *msg, int *buffer, int e, int n) {
    for (int i = 0; i < strlen(msg); ++i) {
        buffer[i] = shifre(msg[i], e, n);
    }
}

void decrypt(int *encMsg, size_t encMsgSize,char *buffer, int d, int n) {
    for (int i = 0; i < encMsgSize; ++i) {
        buffer[i] = shifre(encMsg[i], d, n);
    }
}
