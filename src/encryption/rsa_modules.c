#include "../../include/rsa_modules.h"
#include "math.h"


/**
 * @brief 
 * 
 * @param a 
 * @param b 
 * @return long 
 */
long gcd(long a, long b) {
    if (!b) {
        return a;
    }
    
    return gcd(b, a % b);
}


/**
 * @brief 
 * 
 * @param number 
 * @return char  Так char же, хули 0 или 1? bool тогда уж. #include <stdbool.h>
 */
char is_prime(long number) {
    if (number < 2) {
        return 0;
    }

    for (long i = 2; i < (long)lround(sqrtl(number)) + 1; i++) {
        if (number % i == 0) {
            return 0;
        }
    }

    return 1;
}


/**
 * @brief 
 * 
 * @param a 
 * @param c 
 * @return int 
 */
int modular_inverse(int a, int c) {
    a %= c;
    for (int i = 1; i < c; i++)
    {
        if ((a * i) % c == 1)
            return i;
    }
    return 0;
}