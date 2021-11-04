void print_str(char *str) {
    unsigned int res = 0;

    asm volatile("mov %%eax, %0;" : "=a"(res) : "a"(0), "b"(&str));
    asm volatile("int $0x80;");
}

void read_str(char *str, int cnt) {
    unsigned int arguments[5];
    arguments[0] = str;
    arguments[1] = cnt;

    unsigned int res = 0;

    asm volatile("mov %%eax, %0;" : "=a"(res) : "a"(1), "b"(arguments));
    asm volatile("int $0x80;");
}

unsigned int strlen(const char *str) {
    unsigned int result = 0;
    while (str[result] != 0) {
        result++;
    }
    return result;
}

char* strrev(char *s) {
    unsigned int i, j;
    char c;
    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
    return s;
}

int main() {
    char string1[256];

    print_str("Enter a string: ");

    read_str(string1, 256);
    print_str(strrev(string1));

    return 0;
}
