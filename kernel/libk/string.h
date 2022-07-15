#ifndef _LIBK_STRING_H_
#define _LIBK_STRING_H_

#include <stddef.h>
#include <stdint.h>

int strcmp(const char *, const char *);
int strncmp(const char *, const char *, size_t);

char *strcpy(char *, const char *);
char *strncpy(char *, const char *, size_t);

char *strcat(char *, const char *);
char *strncat(char *, const char *, size_t);

const char *strchr(const char *, char);
char *strrev(char *);

long strtol(char *, char **, int);
char *strtok(char *s, const char *delim);

size_t strlen(const char *);
size_t str_bksp(char *str, char c);

int atoi(const char *);
char *itoa(int, char *, int);

int memcmp(const void *, const void *, size_t);
void *memmove(void *, void *, size_t);

void *memset(void *, uint8_t, size_t);
void *memcpy(void *, const void *, size_t);

#endif
