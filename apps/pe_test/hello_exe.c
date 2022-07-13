#include "printf.h"

const char *test_init = "*** printf.dll test ***";

int main(void) {
    char *s;
    char c;
    int i;
    unsigned u;
    long int l;
    long unsigned n;
    unsigned x;

    for(int i = 0; test_init[i]; i++) {
        putchar(test_init[i]);
    }
    putchar('\n');

    s = "test";
    c = 'X';
    i = -12345;
    u =  12345;
    l = -1234567890;
    n =  1234567890;
    x = 0xABCD;

    printf("String        %s\n", s);
    printf("Char          %c\n", c);
    printf("Integer       %i\n", i);
    printf("Unsigned      %u\n", u);
    printf("Long          %l\n", l);
    printf("Unsigned long %n\n", n);
    printf("Hex           %x\n", x);

    printf("multiple args %s %c %i %u %l %n %x\n", s, c, i, u, l, n, x);
    puts("*** Done ***");
    return 0;
}
