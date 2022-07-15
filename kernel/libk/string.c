#include <kernel/libk/string.h>

#include <stdint.h>
#include <stdbool.h>

#define isdigit(c) ((c) >= '0' && (c) <= '9') ? 1 : 0

size_t strlen(const char *str)
{
    size_t result = 0;
    while (str[result] != 0) {
        result++;
    }
    return result;
}

int atoi(const char *string)
{
    int ret = 0;
    while (isdigit(*string)) {
        ret *= 10;
        ret += (int)(*string - '0');
        string++;
    }
    return ret;
}

char *strrev(char *s)
{
    unsigned int i, j;
    char c;
    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
    return s;
}

char *strcpy(char *s, const char *t)
{
    while ((*s++ = *t++))
        ; // Double brackets to remove the warning - do not remove.
    return s;
}

char *strncpy(char *s, const char *t, size_t n)
{
    size_t i = 0;
    while ((i++ < n) && (*s++ = *t++))
        ;
    return s;
}

char *strcat(char *s, const char *t)
{
    strcpy(s + strlen(s), t);
    return s;
}

char *strncat(char *s, const char *t, size_t n)
{
    strncpy(s + strlen(s), t, n);
    return s;
}

const char *strchr(const char *s, char t)
{
    while (*s) {
        if (*s == t) {
            return s;
        }
        ++s;
    }
    return 0;
}

int strcmp(const char *s1, const char *s2)
{
    for (; *s1 == *s2; s1++, s2++) {
        if (*s1 == '\0') {
            return 0;
        }
    }
    return ((*(unsigned char *)s1 < *(unsigned char *)s2)
                ? -1
                : +1);
}

int strncmp(const char *s1, const char *s2, size_t num)
{
    for (size_t i = 0; i < num; i++) {
        if (s1[i] != s2[i]) {
            return 1;
        }
    }
    return 0;
}

long strtol(char *str, char **endp, int base __attribute__((unused)))
{
    long ret = 0;
    int i = 0;
    while (isdigit(str[i])) {
        ret *= 10;
        ret += str[i] - '0';
    }

    *endp = &str[i];
    if (str[0] == '-') {
        ret = -ret;
    }
    if (str[0] == '+') {
        ret = +ret;
    }
    return ret;
}

char *strpbrk(const char *s, const char *accept)
{
    while (*s != '\0') {
        const char *a = accept;
        while (*a != '\0') {
            if (*a++ == *s) {
                return (char *)s;
            }
        }
        ++s;
    }
    return NULL;
}

size_t strspn(const char *s, const char *accept)
{
    const char *p;
    const char *a;
    size_t count = 0;

    for (p = s; *p != '\0'; ++p) {
        for (a = accept; *a != '\0'; ++a) {
            if (*p == *a) {
                break;
            }
        }
        if (*a == '\0') {
            return count;
        } else {
            ++count;
        }
    }
    return count;
}

char *strtok(char *s, const char *delim)
{
    static char *olds = NULL;
    char *token;

    if (s == NULL) {
        s = olds;
    }

    /* Scan leading delimiters */
    s += strspn(s, delim);
    if (*s == '\0') {
        olds = s;
        return NULL;
    }

    /* Find the end of the token */
    token = s;
    s = strpbrk(token, delim);
    if (s == NULL) {
        /* This token finishes the string */
        olds = token;
    } else {
        /* Terminate the token and make OLDS point past it */
        *s = '\0';
        olds = s + 1;
    }
    return token;
}

size_t str_bksp(char *str, char c)
{
    size_t i = strlen(str);
    i--;
    while (i) {
        i--;
        if (str[i] == c) {
            str[i + 1] = 0;
            return 1;
        }
    }
    return 0;
}

int memcmp(const void *s1, const void *s2, size_t n)
{ /* Length to compare */
    unsigned char u1, u2;

    for (; n--; s1++, s2++) {
        u1 = *(unsigned char *)s1;
        u2 = *(unsigned char *)s2;
        if (u1 != u2) {
            return (u1 - u2);
        }
    }
    return 0;
}

void *memcpy(void *dst, const void *src, size_t n)
{
    /*
    size_t i;
    if ((uint8_t*) (dst) < (uint8_t*) (src)) {
        for (i = 0; i < n; ++i) {
            ((uint8_t*) (dst))[i] = ((uint8_t*) (src))[i];
        }
    } else {
        for (i = 0; i < n; ++i) {
            ((uint8_t*) (dst))[n - i - 1] = ((uint8_t*) (src))[n - i - 1];
        }
    }
    return dst;
    */

    uint32_t num_dwords = n / 4;
    uint32_t num_bytes = n % 4;
    uint32_t *dst32 = (uint32_t *)dst;
    uint32_t *src32 = (uint32_t *)src;
    uint8_t *dst8 = ((uint8_t *)dst) + num_dwords * 4;
    uint8_t *src8 = ((uint8_t *)src) + num_dwords * 4;
    uint32_t i;

    for (i = 0; i < num_dwords; i++) {
        dst32[i] = src32[i];
    }
    for (i = 0; i < num_bytes; i++) {
        dst8[i] = src8[i];
    }

    //asm volatile("cld ; rep movsb" :: "S"(src), "D"(dst), "c"(n) : "flags", "memory");

    return dst;
}

void *memset(void *ptr, uint8_t val, size_t n)
{ // val is uint8_t ????????????
    /*
    size_t i;
    for (i = 0; i < n; ++i) {
        ((uint8_t*) (ptr))[i] = val;
    }
    return ptr;
    */

    uint32_t num_dwords = n / 4;
    uint32_t num_bytes = n % 4;
    uint32_t *dst32 = (uint32_t *)ptr;
    uint8_t *dst8 = ((uint8_t *)ptr) + num_dwords * 4;
    uint8_t val8 = (uint8_t)val;
    uint32_t val32 = val | (val << 8) | (val << 16) | (val << 24);
    uint32_t i;

    for (i = 0; i < num_dwords; i++) {
        dst32[i] = val32;
    }
    for (i = 0; i < num_bytes; i++) {
        dst8[i] = val8;
    }

    return ptr;
}

void *memmove(void *dst, void *src, size_t n)
{
    if (dst != src) {
        memcpy(dst, src, n);
    }
    return dst;
}
