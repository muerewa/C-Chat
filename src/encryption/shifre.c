#include "stdio.h"
#include "string.h"
#include "../../include/shifre.h"


/**
 * @brief 
 * 
 * @param b 
 * @param exp 
 * @param mod 
 * @return long 
 */
long shifre(long b, long exp, long mod) {
    long ret = 1;

    if (mod == 1) {
        return 0;
    }

    b = b % mod;
    
    while (exp > 0) {
        if (exp % 2 == 1) {
            ret = (ret * b) % mod;
        }
        exp = exp >> 1;
        b = (b * b) % mod;
    }
    
    return ret;
}


/**
 * @brief 
 * 
 * @param msg 
 * @param buffer 
 * @param e 
 * @param n 
 */
void encrypt(char *msg, long *buffer, int size, int e, int n) {
    for (int i = 0; i <= size; ++i) {
        buffer[i] = shifre(msg[i], e, n);
    }
}


/**
 * @brief 
 * 
 * @param encMsg 
 * @param encMsgSize 
 * @param buffer 
 * @param d 
 * @param n 
 */
void decrypt(long *encMsg, size_t encMsgSize, char *buffer, int d, int n) {
    for (int i = 0; i < encMsgSize; ++i) {
        buffer[i] = shifre(encMsg[i], d, n);
    }
}
