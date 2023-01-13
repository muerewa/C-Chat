#include "../../include/rsa_modules.h"
#include "math.h"

long gcd(long a, long b) {
    if (b == 0)
        return a;
    else
        return gcd(b, a % b);
}

char is_prime(long number) {
    if (number < 2)
        return 0;
    for (long i = 2; i < (long)lround(sqrtl(number)) + 1; i++)
    {
        if (number % i == 0)
            return 0;
    }
    return 1;
}

int modular_inverse(int a, int c) {
    a %= c;
    for (int i = 1; i < c; i++)
    {
        if ((a * i) % c == 1)
            return i;
    }
    return 0;
}