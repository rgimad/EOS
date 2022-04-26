#include <kernel/libk/math.h>

int pow(int a, int b) {
    if (b <= 0) {
        return 1;
    }
    for (int i = 1; i < b; i++) {
        a *= a;
    }
    return a;
}

double sqrt(double x) {
    int sp = 0;
    int inv = 0;

    double a, b;

    if (x <= 0) {
        return 0;
    }

    if (x < 1.F) {
        x = 1.F / x;
        inv = 1;
    }

    while (x > 16.F) {
        sp++;
        x /= 16.F;
    }

    a = 2.F;

    for (int i = 0; i < 4; i++) {
        b = x / a;
        a += b;
        a *= 0.5F;
    }

    while (sp > 0) {
        sp--;
        a *= 4.F;
    }

    if (inv) {
        a = 1.F / a;
    }

    return a;
}

int floor(double x) {
    return x;
}

int ceil(double x) {
    return x + 0.5;
}

/*
long double ldexp(long double man, int power) {
    return man * pow(2, power);
}
*/

int sign(double a) {
    return a > 0 
           ? 1 
           : a < 0 
             ? -1 
             : 0;
}
