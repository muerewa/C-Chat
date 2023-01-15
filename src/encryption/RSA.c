#include "stdio.h"
#include "time.h"
#include "stdlib.h"
#include "../../include/RSA.h"
#include "../../include/rsa_modules.h"
#include "../../include/shifre.h"
#include "../../include/structures.h"
#include "string.h"

#define MIN 1000
#define MAX 4000

int primeNums[MAX] = {0};
int n = 0;


/**
 * @brief 
 * 
 * @param fi 
 * @return long 
 */
static long	pick_e(long fi) {
    for (long i = 0; i < n; i++)
    {
        if (primeNums[i] % fi != 0 && gcd(primeNums[i], fi) == 1)
            return primeNums[i];
    }

    return 0;
}


/**
 * @brief 
 * 
 */
void fill() {
    for (int i = MIN; i < MAX; ++i) {
        if(is_prime(i)) {
            primeNums[n] = i;
            ++n;
        }
    }
}


/**
 * @brief 
 * 
 * @return int 
 */
int generateKey() {
    srand(time(NULL));
    return primeNums[rand() % n];
}


/**
 * @brief 
 * 
 * @param key 
 */
void generateKeys(struct keys *key) {
    fill();
    int p, q, e, d = 0;
    long fi, n;
    while (d == 0) {
        p = generateKey();
        q = generateKey();
        while (p == q) {
            p = generateKey();
        }
        fi = (p - 1)*(q - 1);
        e = pick_e(fi);
        d = (long)modular_inverse(e, fi);
    }
    n = p * q;
    key->n = n;
    key->e = e;
    key->d = d;
}
