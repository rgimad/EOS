// Graphical demo for EOS

void syscall_draw_square(int x, int y, int width, int height, unsigned int color) {
    unsigned int arguments[5];
    arguments[0] = x;
    arguments[1] = y;
    arguments[2] = width;
    arguments[3] = height;
    arguments[4] = color;

    unsigned int res = 0;

    asm volatile("mov %%eax, %0;" : "=a"(res) : "a"(2), "b"(arguments));
    asm volatile("int $0x80;");
}

int main() {
    int i;
    while (1) {
        for (i = 0; i < 1000; i++) {
            syscall_draw_square(700, 250, 300 - i % 300, 300 - i % 300, 0x00AAAA);
        }
        for (i = 0; i < 1000; i++) {
            syscall_draw_square(700, 250, 300 - i % 300, 300 - i % 300, 0xAA0000);
        }
    }
    return 0;
}
