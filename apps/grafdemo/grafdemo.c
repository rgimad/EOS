// Graphical demo for EOS

typedef struct {
    unsigned int x;
    unsigned int y;
    unsigned int width;
    unsigned int height;
    unsigned int color;
} sys_square_t;

void syscall_draw_square(int x, int y, int width, int height, unsigned int color) {
    sys_square_t args = {x, y, width, height, color};
    __asm__ __volatile__(
        "int $0x80"
        ::"a"(2), "b"(&args)
        :"memory"
    );
}

void print_str(const char *str) {
    unsigned int res = 0;
    asm volatile("mov %%eax, %0;" : "=a"(res) : "a"(0), "b"(&str));
    asm volatile("int $0x80;");
}

char greet[] = "Hello, world!\n";

int main(void) {
    print_str(greet);

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
